#include "tlmm-tests.h"

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <string.h>

/* Define the module info section */
PSP_MODULE_INFO("tlmm test suite", 0, 1, 1);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

/* Define printf, just to make typing easier */
#define printf	pspDebugScreenPrintf

#ifndef TEST_MAX_GROUP
#define TEST_MAX_GROUP 10
#endif/*TEST_MAX_GROUP*/

#ifndef TEST_MAX_TEST
#define TEST_MAX_TEST 20
#endif/*TEST_MAX_TEST*/



_Test::_group* _Test::s_Tests = 0;
#ifdef WIN32
SYSTEMTIME tv_start;
#else
struct timeval tv_start;
#endif

void _Test::Add(const char* name, const char* group, float time)
{
    if(s_Tests == 0)
    {
        s_Tests = new _group[TEST_MAX_GROUP];
	for(int i = 0; i < TEST_MAX_GROUP; ++i)
	{
	    s_Tests[i].name = 0;
	    s_Tests[i].tests = 0;
	}
    }

    _group* grp = 0;
    for(int i = 0; i < TEST_MAX_GROUP; ++i)
	if(s_Tests[i].name && strcmp(s_Tests[i].name, group) == 0)
	{
	    grp = &s_Tests[i];
	    break;
	}
    if(grp == 0)
	for(int i = 0; i < TEST_MAX_GROUP; ++i)
	    if(!s_Tests[i].name)
	    {
		grp = &s_Tests[i];
		grp->name = new char[strlen(group) + 1];
		sprintf(grp->name, group);
		grp->tests = new _test[TEST_MAX_TEST];
		for(int j = 0; j < TEST_MAX_TEST; ++j)
		{
		    grp->tests[j].name = 0;
		    grp->tests[j].test = 0;
		}
		break;
	    }

    if(grp == 0)
    {
	printf("Error adding group {%s}. Not enough free groups\n", group);
	return;
    }

    for(int i = 0; i < TEST_MAX_TEST; ++i)
	if(grp->tests[i].name && strcmp(grp->tests[i].name, name) == 0)
	{
	    grp->tests[i].test = this;
	    grp->tests[i].time = time;
	    return;
	}
    for(int i = 0; i < TEST_MAX_TEST; ++i)
	if(!grp->tests[i].name)
	{
	    grp->tests[i].name = new char[strlen(name)+1];
	    sprintf(grp->tests[i].name, name);
	    grp->tests[i].test = this;
	    grp->tests[i].time = time;
	    return;
	}
    printf("Error adding test [%s] to group {%s}. Not enough free tests\n", name, group);
}

void _Test::RunTest(_test* test, _result* res, int runs)
{
    if(!test || !test->name)
	return;
    printf("    %s", test->name);
    for(unsigned int i = 0; i < 20 - strlen(test->name); ++i) printf(" ");
    test->test->Init();
    float time = test->time;
    Response resp = SUCCESS;
    if(runs == 0)
	resp = test->test->Test(time);
    else
    {
	float target = time;
	float acc = 0.0f;
	for(int i = 0; i < runs; ++i)
	{
	    time = 0.0f;
	    Response r = test->test->Test(time);
	    resp = r == SUCCESS && resp == SUCCESS ? SUCCESS : r;
	    acc += time;
	}	
	time = acc / (float)runs;
	if(target != 0.0f && time > target)
	    resp = resp == SUCCESS ? TIMEOUT : resp;
    }
    printf("%fs", time);
    switch(resp)
    {
    case SUCCESS:
	res->success++;
	printf("\tSUCCESS\n");
	break;
    case TIMEOUT:
	res->fail++;
	printf("\tTIMEOUT\n");
	break;
    case FAILURE:
	res->fail++;
	printf("\tFAILURE\n");
	break;
    }
    test->test->Cleanup();

    res->time += time;
}

void _Test::RunGroup(_group* group, _result* res, int runs)
{
    if(!group || !group->name)
	return;
    printf("\n\n%s\n", group->name);
    //printf("=========================\n");
    _result result;
    for(int i = 0; i < TEST_MAX_TEST; ++i)
    {
	RunTest(&group->tests[i], &result, runs);
    }

    printf("-------------------------\n");
    printf("time: %fs\n", result.time);
    printf("passed: %d\t", result.success);
    printf("failed: %d\n", result.fail);
    printf("-------------------------\n");

    res->time += result.time;
    res->success += result.success;
    res->fail += result.fail;
}

void _Test::RunTests(const char* group, int runs)
{
    if(!s_Tests)
	return;

    _result res;

    if(group)
    {
	for(int i = 0; i < TEST_MAX_GROUP; ++i)
	    if(s_Tests[i].name && 
	       strcmp(s_Tests[i].name, group) == 0)
		RunGroup(&s_Tests[i], &res, runs);
    }
    else
    {
	for(int i = 0; i < TEST_MAX_GROUP; ++i)
	    RunGroup(&s_Tests[i], &res, runs);

	printf("\n-------------------------\n");
	printf(" Results\n");
	printf("-------------------------\n");
	printf("time: %fs\n", res.time);
	printf("passed: %d\t", res.success);
	printf("failed: %d\n", res.fail);
	printf("=========================\n");
    }
}


void dump_threadstatus(void);

int done = 0;

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
	done = 1;
	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
    int thid = 0;
    
    thid = sceKernelCreateThread("update_thread", CallbackThread,
				 0x11, 0xFA0, 0, 0);
    if(thid >= 0)
    {
	sceKernelStartThread(thid, 0, 0);
    }
    
    return thid;
}

const char* Tests[] =
{
    "Basic",
    "Arithmetic",
    "Trigonometry",
    "Equations",
    NULL,
};

int main(void)
{
    gettimeofday(&tv_start, 0);

    SceCtrlData pad;
    int curr = 0;
    int cnt = 1000;
    
    pspDebugScreenInit();
    SetupCallbacks();
    
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    
    while(!done)
    {
	pspDebugScreenSetXY(0, 2);
	sceCtrlReadBufferPositive(&pad, 1);
	printf("Number of tests to run:%d\n", cnt);

	if(pad.Buttons & PSP_CTRL_CROSS)
	    _Test::RunTests(Tests[curr++], cnt);
	if(pad.Buttons & PSP_CTRL_LTRIGGER)
	    cnt--;
	if(pad.Buttons & PSP_CTRL_RTRIGGER)
	    cnt++;
	if(!Tests[curr]) curr = 0;
    }
    
    sceKernelExitGame();
    return 0;
}
