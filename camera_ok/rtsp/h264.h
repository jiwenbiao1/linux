#pragma once
#include "h264_sps.h"
#include "h264_slice.h"

/** Intra frame */
#define BLOCK_FLAG_TYPE_I        0x0002
/** Inter frame with backward reference only */
#define BLOCK_FLAG_TYPE_P        0x0004
/** Inter frame with backward and forward reference */
#define BLOCK_FLAG_TYPE_B        0x0008
	/*
static UINT h264_split_nal(Buffer* p_h264_buff, VBuffer* p_h264_nal_buff_list)
{

		PBYTE& p_data = p_h264_buff->m_pData;
	UINT& i_data_size = p_h264_buff->m_nDataSize;
	
	UINT writeSize = 0;
	
	while( i_data_size > 4 && ( p_data[0] != 0 || p_data[1] != 0 || p_data[2] != 1 ) )
	{
		i_data_size--;
		p_data++;
	}
	
	/// Split nal units
	while( i_data_size > 4 )
	{
		int i_offset;
		int i_size = i_data_size;
		int i_skip = i_data_size;
		
		// search nal end 
		for( i_offset = 4; i_offset+2 < i_data_size ; i_offset++)
		{
			if( p_data[i_offset] == 0 && p_data[i_offset+1] == 0 && p_data[i_offset+2] == 1 )
			{
				//we found another startcode 
				i_size = i_offset - ( p_data[i_offset-1] == 0 ? 1 : 0);
				i_skip = i_offset;
				break;
			} 
		}

		// TODO add STAP-A to remove a lot of overhead with small slice/sei/... 
		if (i_size)
		{
			Buffer* p_nal_buffer = p_h264_nal_buff_list->GetEmptyBuffer();
			if (p_nal_buffer == NULL)
				p_nal_buffer = new Buffer();
			p_nal_buffer->FillData(p_data, i_size);
			p_h264_nal_buff_list->AddFullBuffer(p_nal_buffer);
		}
		
		writeSize += i_size;
		
		i_data_size -= i_skip;
		p_data += i_skip;
	}
	return writeSize;

}
	*/
static void h264_get_nal_type(int* p_nal_type, const uint8_t *p_nal)
{
	int i_nal_hdr;
	
	i_nal_hdr = p_nal[3];
	*p_nal_type = i_nal_hdr&0x1f;
}

static void h264_decode_annexb( uint8_t *dst, int *dstlen,
							   const uint8_t *src, const int srclen )
{
	uint8_t *dst_sav = dst;
	const uint8_t *end = &src[srclen];
	
	while (src < end)
	{
		if (src < end - 3 && src[0] == 0x00 && src[1] == 0x00 &&
			src[2] == 0x03)
		{
			*dst++ = 0x00;
			*dst++ = 0x00;
			
			src += 3;
			continue;
		}
		*dst++ = *src++;
	}
	
	*dstlen = dst - dst_sav;
}




/*
static bool_t h264_find_frame_end(bool_t* p_found_frame_start,
								  const uint8_t *p_nal, const uint32_t nal_length, int i_nal_type)
{
	if (i_nal_type == 1 || i_nal_type == 2 || i_nal_type == 5)
		*p_found_frame_start = TRUE;
	
	if (i_nal_type == 7 || i_nal_type == 8 || i_nal_type == 9)
		*p_found_frame_start = FALSE;
	
	if( (*p_found_frame_start) )
	{
		for (uint32_t i = 3; i < nal_length; i++)
		{
			if (p_nal[i] & 0x80)//////trj note:have problem////must if two first_mb_in_slice////but is correct with this file///
			{
				*p_found_frame_start = FALSE;
				return TRUE;
			}
		}
	}
	
	return FALSE;
}
*/
static int64_t h264_get_pts_rtp(int64_t* p_iframe_offset,
							int64_t frame_count,
							h264_sps_t* p_sps,
							const uint8_t *p_nal, 
							const uint32_t nal_length, 
							int i_nal_type)
{
	// 根据slice type 来判断帧类型，以及计算PTS，DTS
	int64_t i_pts = 0, i_dts = 0;
	int		i_flags = 0;
	
	if (i_nal_type >= 1/*NAL_SLICE*/ && i_nal_type <= 5/*NAL_SLICE_IDR*/)
	{
//		Buffer slice_buff;
//		slice_buff.AllocateBuffer(nal_length);

		unsigned char tmp_buff[100];

		int tmp_size = 0;
		
//		h264_decode_annexb(slice_buff.m_pData, (int*)&slice_buff.m_nDataSize, 
//			p_nal+4, min(nal_length-4, 60));
		/////////////trj added for min function////////////////////////
		int tmpMin = 0;
		if(nal_length-4 >= 60)
		{
			tmpMin = 60;
		}
		else
		{
			tmpMin = nal_length-4;
		}

		h264_decode_annexb(tmp_buff, (int*)&tmp_size, p_nal + 4, tmpMin);

		h264_slice_t slice;
		h264_decode_slice(&slice, tmp_buff, tmp_size, i_nal_type, p_sps);
		
		switch(slice.i_slice_type)
		{
		case 2:	case 7:	
		case 4:	case 9:
			i_flags = BLOCK_FLAG_TYPE_I;
			break;
		case 0:	case 5:
		case 3:	case 8:
			i_flags = BLOCK_FLAG_TYPE_P;
			break;
		case 1:
		case 6:
			i_flags = BLOCK_FLAG_TYPE_B;
			break;
		}
		
		// 按90k的时钟频率计算
		if (i_flags == BLOCK_FLAG_TYPE_I)
		{
			*p_iframe_offset = frame_count;
			i_pts = frame_count * 90000 * 2 * p_sps->num_units_in_tick / p_sps->time_scale;
		}
		else
		{
			i_pts = (slice.i_pic_order_cnt_lsb + (*p_iframe_offset)*2) * 90000 * p_sps->num_units_in_tick / p_sps->time_scale;
		}
	}

	return i_pts;
}