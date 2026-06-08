/*
 * test_task.c
 *
 *  Created on: Jun 4, 2026
 *      Author: serda
 */

#include "cmsis_os.h"
#include "test_runner.h"
#include <stdio.h>

void StartTestTask(void const * argument)
{
  printf("\r\n[TEST] Test task started\r\n");

  RunAllTests();

  printf("\r\n[TEST] Test task finished\r\n");

  for(;;)
  {
    osDelay(1000);
  }
}
