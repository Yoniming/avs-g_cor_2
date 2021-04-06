#include<iostream>
#include "clr.h"
#include "simple_model.h"
#include<algorithm>
#include<vector>
#include<ctime>
#include<cstring>

#define MAX_SYMBOL 42
#define NUM_SYMBOL 38
#define XX_SYMBOL (MAX_SYMBOL-NUM_SYMBOL)
#define TOTAL_LEN 2000000
#define READ_LEN 160

using namespace std;
clock_t start, in_end;

int compress(string input_file, string output_file)
{
	//===============================1.1 Parameter definition & Initialization=======//
	char *quality_block = new char[TOTAL_LEN*READ_LEN];
	char *base_block = new char[TOTAL_LEN*READ_LEN];
	//quality_block = array_char(TOTAL_LEN, READ_LEN);
	//base_block = array_char(TOTAL_LEN, READ_LEN);
	int count_char = 0;
	int rows = 0;
	int model_num = NUM_SYMBOL*NUM_SYMBOL*NUM_SYMBOL * 16;
	char *temp = new char[READ_LEN];
	char * input_buf = new char[READ_LEN + 2];
	char *input_actg = new char[READ_LEN + 2];
	char table[256];
	char *out0 = new char[100000000];

	//memset(out0, 0, 100000000);
	memset(input_buf, 0, READ_LEN + 2);
	memset(input_actg, 0, READ_LEN + 2);

	memset(table, 0, 256);
	table['A'] = 0;
	table['T'] = 1;
	table['C'] = 2;
	table['G'] = 3;
	table['N'] = 4;

	const string input_path = input_file;
	const string output_path = output_file;
	FILE * open_fp;
	if (!(open_fp = fopen(input_path.c_str(), "rb")))
	{
		cout << "Open Failed!" << endl;
		return -1;
	}

	FILE * fp_2;
	if (!(fp_2 = fopen(output_path.c_str(), "wb")))
	{
		cout << "Open Failed!" << endl;
	}
	//===============================1.2 Encoder definition=========================//
	int adaptive_flag = 1;
	int T = XX_SYMBOL - 1;

	//RangeCoder rc;
	//rc.output(out0);
	//rc.StartEncode();

	SIMPLE_MODEL<NUM_SYMBOL+1> *model_qual;
	model_qual = new SIMPLE_MODEL<NUM_SYMBOL+1>[model_num + 1];



	//ac_encoder ace_ny;
	//ac_model * mean_model = new ac_model[NUM_SYMBOL];
	//ac_model * acm_ny = new ac_model[model_num+1];

	//ac_encoder_init(&ace_ny, "result.stream");
	//for (int i = 0; i < NUM_SYMBOL; i++)
	//	ac_model_init(&mean_model[i], NUM_SYMBOL, NULL, adaptive_flag);
	//for (int i = 0; i < model_num; i++)
	//{
	//	ac_model_init(&acm_ny[i], NUM_SYMBOL+1, NULL, adaptive_flag);
	//}
	//ac_model_init(&acm_ny[model_num], XX_SYMBOL + 1, NULL, adaptive_flag);

	//=====================================2 Process==============================//
	char *quality_ptr = quality_block;
	char *base_ptr = base_block;
	int total_sz = 0;
	while (!feof(open_fp)) // loop for each canvas
	{
		char * tmp;

		for (int i = 0; i < TOTAL_LEN; i++)
		{
			fgets(temp, READ_LEN, open_fp);
			fgets(input_actg, READ_LEN+2, open_fp);
			fgets(temp, READ_LEN, open_fp);
			tmp = fgets(input_buf, READ_LEN+2, open_fp);
			if (tmp == NULL)
			{
				break;
			}
			count_char = strlen(input_buf) - 1;
			if (count_char > READ_LEN)
			{
				cout << "Exceeded predetermined length! Length needs to be less than " << READ_LEN << endl;
			}
			memcpy(quality_ptr, input_buf, count_char);
			quality_ptr += count_char;
			
			memcpy(base_ptr, input_actg, count_char);
			base_ptr += count_char;
			rows++;
		}

		int cols = count_char;
		RangeCoder rc;
		rc.output(out0+4);
		rc.StartEncode();

		//SIMPLE_MODEL<NUM_SYMBOL> *model_qual;
		//model_qual = new SIMPLE_MODEL<NUM_SYMBOL>[model_num + 1];
		//for (int i = 0; i < rows; i++)
		//{
		//	for (int j = 0; j < cols; j++)
		//	{
		//		quality_block[i*cols + j] = quality_block[i*cols + j] - '!';
		//	}
		//}


		//int max_1 = 0, min_1 = 100;
		//for (int i = 0; i < rows; i++)
		//{
		//	for (int j = 0; j < cols; j++)
		//	{
		//		if (int(quality_block[i*cols + j]) >max_1)
		//			max_1 = int(quality_block[i*cols + j]);
		//	}
		//}
		//for (int i = 0; i < rows; i++)
		//{
		//	for (int j = 0; j < cols; j++)
		//	{
		//		if (int(quality_block[i*cols + j]) <min_1)
		//			min_1 = int(quality_block[i*cols + j]);
		//	}
		//}
		//cout << max_1 << endl << min_1;


		//for (int i = 0; i < rows; i++)
		//{
		//	for (int j = 0; j < cols; j++)
		//	{
		//		if (int(quality_block[i*cols + j])==33)
		//		{
		//			cout <<"i"<<i<<endl<<"j"<<j<<endl;
		//		}
		//	}
		//}

		int i = 0, offset, model_idx;
		for (int j = 0; j < cols; j++)
		{
			for (int k = 0; k < rows; k++)
			{
				if (j % 2 == 0)
				{
					i = k;
				}
				else
				{
					i = rows - k - 1;
				}
				offset = i*cols + j;
				quality_block[offset] = quality_block[offset] - '!';
				char cur_base = base_block[offset];
				int J0 = table[cur_base];
				if (J0 == 4)
				{
					model_idx = 0;
				}
				else
				{
					if (j == 0)
					{
						model_idx = J0 + 1;
					}
					else if (j == 1)
					{
						char pre_base_1 = base_block[offset - 1];
						int J1 = table[pre_base_1];
						int G1 = J0 * 5 + J1;
						int Q1 = quality_block[offset - 1];
						model_idx = Q1 * 20 + G1 + 5;
					}
					else if (j == 2)
					{
						char pre_base_1 = base_block[offset - 1];
						int J1 = table[pre_base_1];
						int G1 = J0 * 5 + J1;
						int Q1 = quality_block[offset - 1];
						int Q2 = quality_block[offset - 2];
						model_idx = (Q1*NUM_SYMBOL + Q2) * 20 + G1 + 5 + NUM_SYMBOL * 20;
					}
					else
					{
						char pre_base_1 = base_block[offset - 1];
						char pre_base_2 = base_block[offset - 2];
						int J1 = table[pre_base_1];
						int J2 = table[pre_base_2];
						int G1 = J0 * 5 + J1;
						int G2 = J0 * 4 * 4 + J1 * 4 + J2;
						int Q1 = quality_block[offset - 1];
						int Q2 = quality_block[offset - 2];
						int Q3 = quality_block[offset - 3];
						int Q4 = 0;
						if (j == 3)
						{
							Q4 = Q3;
						}
						else
						{
							int Q4 = quality_block[offset - 4];
						}
						int A = Q1;
						int B = max(Q2, Q3);
						int C = 0;
						if (Q2 == Q3)
						{
							C = 1;
						}
						if (Q3 == Q4)
						{
							C = 2;
						}
						int M = j / 13;
						int a =  cols/ 13 + 1;
						model_idx = A * NUM_SYMBOL * 3 * a * 20 + B * 3 * a * 20 + C*a * 20 + M * 20 + G1 + 5 + NUM_SYMBOL * 20 + NUM_SYMBOL*NUM_SYMBOL * 20;
					}
				}
				//model_qual[model_idx].encodeSymbol(&rc, quality_block[offset]);
				if (quality_block[offset]>T)
				{
					//ac_encode_symbol(&ace_ny, &acm_ny[model_idx], quality_block[offset] - T);
					model_qual[model_idx].encodeSymbol(&rc, quality_block[offset]-T);
				}
				else
				{
					//ac_encode_symbol(&ace_ny, &acm_ny[model_idx], 0);
					//ac_encode_symbol(&ace_ny, &acm_ny[model_num], quality_block[offset]);
					model_qual[model_idx].encodeSymbol(&rc, 0);
					model_qual[model_num].encodeSymbol(&rc, quality_block[offset]);
				}
			}
		}
		rows = 0;
		rc.FinishEncode();
		int sz1; 
		sz1 = rc.size_out();
		total_sz += sz1;
		out0[0] = (sz1 >> 0) & 0xff;
		out0[1] = (sz1 >> 8) & 0xff;
		out0[2] = (sz1 >> 16) & 0xff;
		out0[3] = (sz1 >> 24) & 0xff;
		fwrite(out0, sizeof(char), sz1+4, fp_2);


		//memset(quality_block, 0, TOTAL_LEN*READ_LEN);
		//memset(base_block, 0, TOTAL_LEN*READ_LEN);
		quality_ptr = quality_block;
		base_ptr = base_block;
		//out0_ptr = out0;
		
	}
	//ac_encoder_done(&ace_ny);
	//for (int i = 0; i < model_num; i++)
	//{
	//	ac_model_done(&acm_ny[i]);
	//}
	//rc.FinishEncode();
	//int sz1;
	//sz1 = rc.size_out();


	//fwrite(out0, sizeof(char), sz1, fp_2);
	fclose(fp_2);

	fclose(open_fp);
	delete[] model_qual;
	delete[] quality_block;
	delete[] base_block;
	delete[] temp;
	delete[] input_buf;
	delete[] input_actg;
	delete[] out0;
	return total_sz;
}
int main(int argcc, char* argv[])
{
	start = clock();
	string input_file = argv[1];
	string output_file = argv[2];
	int result=compress(input_file, output_file);
	cout << "The size of file is: " << result << endl;
	in_end = clock();
	double endtime = (double)(in_end - start) / CLOCKS_PER_SEC;
	cout << "Total time:" << endtime << "s" << endl;
	getchar();
	return 0;
}