#include <cstdlib> // for EXIT_SUCCESS
#include <cstring>
#include <stdint.h>
#include <cstdio>
#include <assert.h>
#include <sys/time.h>
#define SIZE 10000000

void radixSort(uint32_t *array, uint32_t *aux, int length);
void quickSort(uint32_t *array, int start, int end);

int main()
{
	struct timeval startTime;
	struct timeval endTime;
	uint32_t *array = new uint32_t[SIZE];
	uint32_t *auxiliary = new uint32_t[SIZE];


	for (int i = 0; i < 10; i++)
	{
		gettimeofday(&startTime, NULL);
//		radixSort(array, auxiliary, SIZE);
		quickSort(array, 0, SIZE);
		gettimeofday(&endTime, NULL);

		unsigned long int ticks = (endTime.tv_sec - startTime.tv_sec) * 1000 +
		                          (endTime.tv_usec - startTime.tv_usec) / 1000;
//		printf("Sorted %d numbers in %lums \n", SIZE, ticks);
		printf("%lu ", ticks);
	}

	// Checking order
	for (int i = 1; i < SIZE; i++)
	{
		assert(array[i - 1] <= array[i]);
	}

	delete[] array;
	delete[] auxiliary;

	return EXIT_SUCCESS;
}

void radixSort(uint32_t *array, uint32_t *aux, int length)
{
	uint32_t histograms[256 * 4];
	memset(histograms, 0, 256 * 4 * sizeof(uint32_t));
	uint32_t *histogram[4] = {&histograms[0], &histograms[256], &histograms[256 * 2], &histograms[256 * 3]};

	uint32_t offsetTables[256 * 4];
	uint32_t *offsetTable[4] = {&offsetTables[0], &offsetTables[256], &offsetTables[256 * 2], &offsetTables[256 * 3]};

	for (int currentItem = 0; currentItem < length; ++currentItem)
	{
		const uint32_t value = array[currentItem];
		const uint32_t value0 = value & 0xff;
		const uint32_t value1 = (value >> 8) & 0xff;
		const uint32_t value2 = (value >> 16) & 0xff;
		const uint32_t value3 = (value >> 24);
		histogram[0][value0]++;
		histogram[1][value1]++;
		histogram[2][value2]++;
		histogram[3][value3]++;
	}

	offsetTable[0][0] = 0;
	offsetTable[1][0] = 0;
	offsetTable[2][0] = 0;
	offsetTable[3][0] = 0;

	for (int i = 1; i < 256; i++)
	{
		offsetTable[0][i] = offsetTable[0][i - 1] + histogram[0][i - 1];
		offsetTable[1][i] = offsetTable[1][i - 1] + histogram[1][i - 1];
		offsetTable[2][i] = offsetTable[2][i - 1] + histogram[2][i - 1];
		offsetTable[3][i] = offsetTable[3][i - 1] + histogram[3][i - 1];
	}


	uint32_t *arrayPointers[3] = {aux, array, NULL};
	for (int nPass = 0; nPass < 4; nPass++)
	{
		// Swapping target pointers
		arrayPointers[2] = arrayPointers[0];
		arrayPointers[0] = arrayPointers[1];
		arrayPointers[1] = arrayPointers[2];

		for (int currentValue = 0; currentValue < length; ++currentValue)
		{
			const uint32_t value = arrayPointers[0][currentValue];
			const uint32_t offsetTableIndex = (value >> 8 * nPass) & 0xff;
			const uint32_t writePosition = offsetTable[nPass][offsetTableIndex];
			offsetTable[nPass][offsetTableIndex]++;
			arrayPointers[1][writePosition] = value;
		}
	}
}

int partition(uint32_t *array, int start, int end)
{
	int i, j, quit;
	uint32_t pivot;

	pivot = array[start];
	i = start - 1;
	j = end + 1;

	quit = 1;

	do
	{
		do
		{
			j--;
		} while (array[j] > pivot);

		do
		{
			i++;
		} while (array[i] < pivot);

		if (i < j)
		{
			int temp = array[i];
			array[i] = array[j];
			array[j] = temp;
		}
		else
		{
			quit = 0;
		}
	} while (quit);

	return j;
}


/* Quick sort implementation from start to end of array */
void quickSort(uint32_t *array, int start, int end)
{
	if (start < end)
	{
		int div = partition(array, start, end);
		quickSort(array, start, div);
		quickSort(array, div + 1, end);
	}
}
