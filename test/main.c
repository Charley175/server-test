/*************************************************************************
	> File Name: main.c
	> Author: Charley
	> Mail: joke_lover@163.com 
	> Created Time: 2021年04月14日 星期三 15时02分35秒
 ************************************************************************/

#include<stdio.h>
#include "threadpool.h"
#include <pthread.h>

int j = 100;

void *do_work(void *a)
{
	printf("CCC : %lx : %d\n", pthread_self(),j--);
	sleep(2);
}


int main()
{ /* 线程池初始化，其管理者线程及工作线程都会启动 */
    threadpool_t *thp = threadpool_create(1, 20, 50);
    printf("threadpool init ... ... \n");

	int p =1; 

	sleep(2);
	int i =0;
	for ( i = 0; i < 50; ++i)
   /* 接收到任务后添加 */
		threadpool_add_task(thp, do_work, (void *)&i);

   // ... ...
	while (i--)
	{
       sleep(10);	
	}
	sleep(5);
   /* 销毁 */
    threadpool_destroy(thp);
}
