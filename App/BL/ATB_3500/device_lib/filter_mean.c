/**
 * @file filter_mean.c
 * @author a.bodin
 * @date 2024-01-18
 */

#include "filter_mean.h"
/*******************************************************************************
//  Фильтрация 
 *******************************************************************************/
int16_t filter_mean (filter_mean_t *str, int16_t input)
{
	if(!str->ok)
		return 0;
	uint8_t new_sample_available = 1;
	if(str->get_ms_time_from != NULL && str->get_ms_time_from != NULL) {
		if(!(str->get_ms_time_from(str->time_stamp) >= str->sample_time_ms)) {
			new_sample_available = 0;
		}
	}
	if(new_sample_available) {
		// Инкрементирование нового члена
		str->sum += input;
		// Декрементирование старого члена
		str->sum -= str->array[str->cur_item_number];
		// Запись нового значения
		str->array[str->cur_item_number] = input;
		// Инкрементирование счётчика 
		if ((++(str->cur_item_number)) >= str->lenght)
		{
			str->cur_item_number = 0;
			str->is_clear = false;
		} 
		str->time_stamp = str->get_ms_time();
		// Возвращение отфильтрованного значения 
		str->result = str->sum / str->lenght;
	}
	else
		str->result = str->sum / str->lenght;
	return str->result;
}


int32_t filter_mean_get(filter_mean_t *str)
{
	if(!str->ok)
		return 0;
	return str->result;
}


/*******************************************************************************
//  Инициализация структуры фильтрации 
 *******************************************************************************/
uint8_t filter_mean_init  (filter_mean_t *str, uint16_t lenght, uint32_t sample_time_ms, get_ms_time_from_fn  get_ms_time_from_ptr, get_ms_time_fn get_ms_time_ptr)
{
	if (lenght > MAX_FILTER_MEAN_LENGH)
		lenght = MAX_FILTER_MEAN_LENGH;

	str->get_ms_time_from = get_ms_time_from_ptr;
	str->get_ms_time = get_ms_time_ptr;
	str->sample_time_ms = sample_time_ms;
	str->lenght = lenght;               // Количество
	str->ok = true;
	filter_mean_clear(str);
	return true;
}


void filter_mean_clear(filter_mean_t *str)
{	
	str->sum = 0;
	str->cur_item_number = 0;
	for (int i = 0; i < str->lenght; i++)
	{
		str->array[i] = 0;
	}
	str->is_clear = true;
}

uint8_t filter_mean_is_full(filter_mean_t *str)
{
	return (str->is_clear == 0) ? 1 : 0;
}

