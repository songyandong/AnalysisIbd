//����1���Զ�����ģʽ�򿪲�д���ļ�
#include <stdio.h>
#include <stdlib.h>  
#include <iostream>
#include <cstring>
#include "fsp.h"
#include "pag0page.h"

using namespace std;


typedef struct
{
	uint addr_page;
	uint addr_offset;
}point_info;

typedef struct
{
	uint flst_len;  //������
	point_info flst_first; //�����һ����ָ��
	point_info flst_last; //������������һ����ָ��
}fsp_info;


typedef struct
{
	uint page_ssize;
	uint space_id; //��ռ�id
	uint page_size; //ҳ��С
	uint free_len; 
	uint free_limit;
	bool is_com;
	uint fil_page_type; //����
	uint fsp_space_id; //��ռ�id
	uint fsp_not_used; //δʹ�õĴ�����
	uint fsp_size; //��ռ���ҳ��
	uint fsp_space_flags; //flags ��λ���space_id��ѹ��ҳ��С,ѹ�����͵�
	uint fsp_frag_n_used; //FSP_FREE_FRAG���Ѿ���ʹ�õ�ҳ����
	fsp_info fsp_free; //ƫ��FSP_FREE + FLST_LEN,д��0ռ��4bytes,ƫ��FSP_FREE + FLST_FIRST/FLST_LAST + FIL_ADDR_PAGE��4bytes��¼ָ����Ϣ
					//FSP_FREE + FLST_FIRST/FLST_LAST + FIL_ADDR_BYTEռ��2bytes��offset��Ϣ�� ��¼��ռ������п��дص�������Ϣ
	fsp_info fsp_free_frag;//��FSP_FREE���ƣ�ֻ�����Ǽ�¼�Ĵ�Ϊ�Ѿ�����ʹ�õģ������ǿմ��ֲ������ص�����
	fsp_info fsp_full_frag;//ͬ�ϣ�������˼������洢��Ϊ����
	fsp_info fsp_seg_inodes_full;//ͬ�ϣ����ڴ�Ź������ݶε�ҳ��������ָ��Ϊ����������ҳ����
	fsp_info fsp_seg_inodes_free;//ͬ�ϣ����ָ��Ϊ����������ݶι�����Ϣ������ҳ����
	uint fsp_seg_id;//ƫ��FSP_SEG_ID��¼��ID��ռ��1bytes�����ݶ�ID
}Fsp_Info;


void Print_content(Fsp_Info *fsp_info_value);

//��ȡpage_size
int page_size_t(uint fsp_flags)
{
	uint	ssize = FSP_FLAGS_GET_PAGE_SSIZE(fsp_flags);

	/* If the logical page size is zero in fsp_flags, then use the
	legacy 16k page size. */
	ssize = (0 == ssize) ? UNIV_PAGE_SSIZE_ORIG : ssize;

	/* Convert from a 'log2 minus 9' to a page size in bytes. */
	const uint	size = ((UNIV_ZIP_SIZE_MIN >> 1) << ssize);

	return size;
}

//convert the byte of Big-Endian and Little-Endian
int conversion_byte_order(int num)
{
	uint tmp;
	tmp = ((num & 0x000000FF) << 24) |
		((num & 0x0000FF00) << 8) |
		((num & 0x00FF0000) >> 8) |
		((num & 0xFF000000) >> 24);
	return tmp;
}

int check_unside(uint value)
{
	if (value == -1)
	{
		return 0;
	}
	else
		return value;
}

int conversion_byte_order_two(short num)
{
	uint tmp;
	tmp = ((num & 0x00FF) << 8) |
		((num & 0xFF00) >> 8);
	return tmp;
}

//��ȡ4bytes
int read_int(FILE *file)
{
	int value;
	fread(&value, sizeof(int), 1, file);
	return value;
}

//��ȡ�ء��ε�ָ����Ϣ
void *read_fsp_content(Fsp_Info *fsp_info_value, char* buffer, uint offset, int type)
{
	uint flst_len, flst_first_page, flst_first_page_offset, flst_last_page, flst_last_page_offset;
	memcpy(&flst_len, buffer + offset + FLST_LEN, 4);
	memcpy(&flst_first_page, buffer + offset + FLST_FIRST + FIL_ADDR_PAGE, 4);
	memcpy(&flst_first_page_offset, buffer + offset + FLST_FIRST + FIL_ADDR_BYTE, 2);
	memcpy(&flst_last_page, buffer + offset + FLST_LAST + FIL_ADDR_PAGE, 4);
	memcpy(&flst_last_page_offset, buffer + offset + FLST_LAST + FIL_ADDR_BYTE, 2);
	
	flst_len = check_unside(conversion_byte_order(flst_len));
	flst_first_page = check_unside(conversion_byte_order(flst_first_page));
	flst_first_page_offset = check_unside(conversion_byte_order_two(flst_first_page_offset));
	flst_last_page = check_unside(conversion_byte_order(flst_last_page));
	flst_last_page_offset = check_unside(conversion_byte_order_two(flst_last_page_offset));
	
	if (type == 1)
	{
		fsp_info_value->fsp_free.flst_len = flst_len;
		fsp_info_value->fsp_free.flst_first.addr_page = flst_first_page;
		fsp_info_value->fsp_free.flst_first.addr_offset = flst_first_page_offset;
		fsp_info_value->fsp_free.flst_last.addr_page = flst_last_page;
		fsp_info_value->fsp_free.flst_last.addr_offset = flst_last_page_offset;
	}
	else if (type == 2)
	{
		fsp_info_value->fsp_free_frag.flst_len = flst_len;
		fsp_info_value->fsp_free_frag.flst_first.addr_page = flst_first_page;
		fsp_info_value->fsp_free_frag.flst_first.addr_offset = flst_first_page_offset;
		fsp_info_value->fsp_free_frag.flst_last.addr_page = flst_last_page;
		fsp_info_value->fsp_free_frag.flst_last.addr_offset = flst_last_page_offset;
	}
	else if (type == 3)
	{
		fsp_info_value->fsp_full_frag.flst_len = flst_len;
		fsp_info_value->fsp_full_frag.flst_first.addr_page = flst_first_page;
		fsp_info_value->fsp_full_frag.flst_first.addr_offset = flst_first_page_offset;
		fsp_info_value->fsp_full_frag.flst_last.addr_page = flst_last_page;
		fsp_info_value->fsp_full_frag.flst_last.addr_offset = flst_last_page_offset;
	}
	else if (type == 4)
	{
		fsp_info_value->fsp_seg_inodes_free.flst_len = flst_len;
		fsp_info_value->fsp_seg_inodes_free.flst_first.addr_page = flst_first_page;
		fsp_info_value->fsp_seg_inodes_free.flst_first.addr_offset = flst_first_page_offset;
		fsp_info_value->fsp_seg_inodes_free.flst_last.addr_page = flst_last_page;
		fsp_info_value->fsp_seg_inodes_free.flst_last.addr_offset = flst_last_page_offset;
	}
	else if (type == 5)
	{
		fsp_info_value->fsp_seg_inodes_full.flst_len = flst_len;
		fsp_info_value->fsp_seg_inodes_full.flst_first.addr_page = flst_first_page;
		fsp_info_value->fsp_seg_inodes_full.flst_first.addr_offset = flst_first_page_offset;
		fsp_info_value->fsp_seg_inodes_full.flst_last.addr_page = flst_last_page;
		fsp_info_value->fsp_seg_inodes_full.flst_last.addr_offset = flst_last_page_offset;
	}

}



void Enter(char *file_name)
{
	FILE *fp = fopen(file_name, "rb+");   //������ģʽ
	if (NULL == fp)
	{
		cout << file_name << ": No such file or directory" << endl;
		exit;
	}

	uint tmp_value;
	Fsp_Info *fsp_info_value = new Fsp_Info;

	/* ��headerҳ����buffer*/
	char *buffer;
	buffer = new char[UNI_PAGE_SIZE];
	fread(buffer, UNI_PAGE_SIZE, 1, fp);
	

	//page_ssize
	memcpy(&tmp_value,buffer + FSP_HEADER_OFFSET + FSP_SPACE_FLAGS,4);
	fsp_info_value->page_ssize = FSP_FLAGS_GET_PAGE_SSIZE(tmp_value);

	//page_size
	memcpy(&tmp_value, buffer + FSP_HEADER_OFFSET + FSP_SPACE_FLAGS, 4);
	fsp_info_value->page_size = page_size_t(tmp_value);

	//size_in_header
	memcpy(&tmp_value, buffer + FSP_HEADER_OFFSET + FSP_SIZE, 4);
	fsp_info_value->fsp_size = conversion_byte_order(tmp_value);

	//space_id
	memcpy(&tmp_value, buffer + FSP_HEADER_OFFSET, 4);
	fsp_info_value->space_id = conversion_byte_order(tmp_value);

	//id
	memcpy(&tmp_value, buffer + FIL_PAGE_ARCH_LOG_NO_OR_SPACE_ID, 4);
	fsp_info_value->fsp_space_id = conversion_byte_order(tmp_value);

	//free_limit
	memcpy(&tmp_value, buffer + FSP_HEADER_OFFSET + FSP_FREE_LIMIT, 4);
	fsp_info_value->free_limit = conversion_byte_order(tmp_value);

	//free_len
	memcpy(&tmp_value, buffer + FSP_HEADER_OFFSET + FSP_FREE + FLST_LEN, 4);
	fsp_info_value->free_len = conversion_byte_order(tmp_value);

	//page_type_fsp_hdr
	memcpy(&tmp_value, buffer + FIL_PAGE_TYPE, 2);
	fsp_info_value->fil_page_type = conversion_byte_order(tmp_value);

	//fsp_not_used
	memcpy(&tmp_value, buffer + FSP_HEADER_OFFSET + FSP_NOT_USED, 4);
	fsp_info_value->fsp_not_used = conversion_byte_order(tmp_value);

	//fsp_flags
	memcpy(&tmp_value, buffer + FSP_HEADER_OFFSET + FSP_SPACE_FLAGS, 4);
	fsp_info_value->fsp_space_flags = tmp_value;

	//fsp_frag_n_used
	memcpy(&tmp_value, buffer + FSP_HEADER_OFFSET + FSP_FRAG_N_USED, 4);
	fsp_info_value->fsp_frag_n_used = conversion_byte_order(tmp_value);

	//is_com �Ƿ�ѹ��
	fsp_info_value->is_com = page_is_comp(buffer);

	/*���ڶ�ָ��*/
	//���д���Ϣ
	read_fsp_content(fsp_info_value, buffer , FSP_HEADER_OFFSET + FSP_FREE,1);
	//��մ���Ϣ
	read_fsp_content(fsp_info_value, buffer , FSP_HEADER_OFFSET + FSP_FREE_FRAG,2);
	//������Ϣ
	read_fsp_content(fsp_info_value, buffer , FSP_HEADER_OFFSET + FSP_FULL_FRAG,3);

	//���ݶ���Ϣ
	//����������ҳ������Ϣ
	read_fsp_content(fsp_info_value, buffer , FSP_HEADER_OFFSET + FSP_SEG_INODES_FULL,4);
	//����������ҳ������Ϣ
	read_fsp_content(fsp_info_value, buffer , FSP_HEADER_OFFSET + FSP_SEG_INODES_FREE, 5);


	delete []buffer;
	fclose(fp);
	Print_content(fsp_info_value);
	delete[]fsp_info_value;
}

void Print_content(Fsp_Info *fsp_info_value)
{
	cout << "page_ssize: " << fsp_info_value->page_ssize << endl;
	cout << "page_size: " << fsp_info_value->page_size << "bytes" << endl;
	cout << "size_in_header: " << fsp_info_value->fsp_size << endl;
	cout << "space_id: " << fsp_info_value->space_id << endl;
	cout << "id: " << fsp_info_value->fsp_space_id << endl;
	cout << "free_limit: " << fsp_info_value->free_limit << endl;
	cout << "free_len: " << fsp_info_value->free_len << endl;
	cout << "fsp_type_fsp_hdr: " << fsp_info_value->fil_page_type << endl;
	cout << "fsp_not_used: " << fsp_info_value->fsp_not_used << endl;
	cout << "fsp_flags: " << fsp_info_value->fsp_space_flags << endl;
	cout << "fsp_frag_n_used: " << fsp_info_value->fsp_frag_n_used << endl;

	//����Ϣ
	cout << "fsp_free->  len: " << fsp_info_value->fsp_free.flst_len;
	cout << " first -> page_number: " << fsp_info_value->fsp_free.flst_first.addr_page;
	cout << " page_offset: " << fsp_info_value->fsp_free.flst_first.addr_offset;
	cout << " last -> page_number: " << fsp_info_value->fsp_free.flst_last.addr_page;
	cout << " page_offset: " << fsp_info_value->fsp_free.flst_last.addr_offset << endl;

	cout << "fsp_free_frag->  len: " << fsp_info_value->fsp_free_frag.flst_len;
	cout << " first -> page_number: " << fsp_info_value->fsp_free_frag.flst_first.addr_page;
	cout << " page_offset: " << fsp_info_value->fsp_free_frag.flst_first.addr_offset;
	cout << " last -> page_number: " << fsp_info_value->fsp_free_frag.flst_last.addr_page;
	cout << " page_offset: " << fsp_info_value->fsp_free_frag.flst_last.addr_offset << endl;

	cout << "fsp_full_frag->  len: " << fsp_info_value->fsp_full_frag.flst_len;
	cout << " first -> page_number: " << fsp_info_value->fsp_full_frag.flst_first.addr_page;
	cout << " page_offset: " << fsp_info_value->fsp_full_frag.flst_first.addr_offset;
	cout << " last -> page_number: " << fsp_info_value->fsp_full_frag.flst_last.addr_page;
	cout << " page_offset: " << fsp_info_value->fsp_full_frag.flst_last.addr_offset << endl;


	//���ݶ���Ϣ
	cout << "fsp_seg_free-> len: " << fsp_info_value->fsp_seg_inodes_free.flst_len;
	cout << " first -> page_number: " << fsp_info_value->fsp_seg_inodes_free.flst_first.addr_page;
	cout << " page_offset: " << fsp_info_value->fsp_seg_inodes_free.flst_first.addr_offset;
	cout << " last -> page_number: " << fsp_info_value->fsp_seg_inodes_free.flst_last.addr_page;
	cout << " page_offset: " << fsp_info_value->fsp_seg_inodes_free.flst_last.addr_offset << endl;

	cout << "fsp_seg_full-> len: " << fsp_info_value->fsp_seg_inodes_full.flst_len;
	cout << " first -> page_number: " << fsp_info_value->fsp_seg_inodes_full.flst_first.addr_page;
	cout << " page_offset: " << fsp_info_value->fsp_seg_inodes_full.flst_first.addr_offset;
	cout << " last -> page_number: " << fsp_info_value->fsp_seg_inodes_full.flst_last.addr_page;
	cout << " page_offset: " << fsp_info_value->fsp_seg_inodes_full.flst_last.addr_offset << endl;
}

int main(int argc, char ** argv)
{
	if (argc > 1)
	{
		for (int i = 1; i<argc; i++)
		{
			if (string(argv[i]) == "-h")
				cout << "Usage: [-h] [-file file_path] " << endl;
			else if (string(argv[i]) == "-file")
			{
				if (argc < 3)
				{
					cout << "Usage: [-h] [-file file_path] " << endl;
					break;
				}
				else
				{
					char *file_name = argv[i+1];
					Enter(file_name);
					return 1;
				}
			}
			else cout << "Usage: [-h] [-file file_path] " << endl;
		}
	}
	else
	{
		cout << "Usage: [-h] [-file file_path] " << endl;

	}
	return 0;
}