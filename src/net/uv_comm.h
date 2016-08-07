#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <assert.h>
#include <uv.h>

const char* uv_comm_get_tcp_ip_port(uv_tcp_t* uvclient, char* ipbuf, int buflen, int* port) {
	struct sockaddr addr;
	int len = sizeof(addr);

	/* �ȵ���uv�ӿڻ�ȡ��struct sockaddr��̬�ĵ�ַ */
	int r = uv_tcp_getpeername(uvclient, &addr, &len);
	if(r == 0) {
	    /* ת��Ϊ�ɶ�����ip�ַ����Ͷ˿� */
		return uv_comm_get_ip_port(&addr, ipbuf, buflen, port);
	} else {
        printf("\n!!! [cmm] get client ip fails: %s\n", uv_strerror(r));
		return NULL;
	}
}

/* uv_comm_send_to_stream�Ľ������ص����� */
static void uv_comm_after_send_to_stream(uv_write_t* w, int status) {
    /* status Ϊ0ʱ, send_to_stream�ɹ�, �����������ʧ�� */
    if(status) {
        puts("\n!!! [cmm] uv_comm_after_send_to_stream(,-1) failed");
    }
    //�ͷ���uv_comm_send_to_stream������ڴ�
    free(w->data);
    free(w);
}

int uv_comm_send_to_stream(uv_stream_t* stream, void* data, unsigned int size) {
    assert(stream && data);
    uv_buf_t buf;
    /* ���촫�ݸ�uv_write��buf */
    buf.base = (char*)data;
    buf.len = (size_t)size;

    /* ���촫�ݸ�����ص�����uv_comm_after_send_to_streamuv_write_t */
    uv_write_t* w = (uv_write_t*) malloc(sizeof(uv_write_t));
    memset(w, 0, sizeof(uv_write_t));
    w->data = data;

    /* ����uv_writeд������ */
    return (uv_write(w, stream, &buf, 1, uv_comm_after_send_to_stream) == 0 ? 1 : 0);
}

void uv_comm_on_alloc_buf(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
	buf->base = (char*) malloc(suggested_size);
	buf->len  = buf->base ? suggested_size : 0;
}
