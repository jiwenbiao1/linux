#include <setup.h>

static struct tag *params;
static void setup_start_tag (void)
{
	params = (struct tag *)0x50000100;

	params->hdr.tag = ATAG_CORE;
	params->hdr.size = tag_size (tag_core);

	params->u.core.flags = 0;
	params->u.core.pagesize = 0;
	params->u.core.rootdev = 0;

	params = tag_next (params);
}

static void setup_memory_tags (void)
{
	params->hdr.tag = ATAG_MEM;
	params->hdr.size = tag_size (tag_mem32);

	params->u.mem.start = 0x50000000;
	params->u.mem.size = 128*1024*1024;

	params = tag_next (params);
}

char * strcpy(char * dest,const char *src)
{
	char *tmp = dest;

	while ((*dest++ = *src++) != '\0')
		/* nothing */;
	return tmp;
}
int strlen(const char * s){	const char *sc;	for (sc = s; *sc != '\0'; ++sc)		/* nothing */;	return sc - s;}
static void setup_commandline_tag (void)
{
	char *commandline = "root=/dev/nfs nfsroot=10.1.1.119:/share/wei/rootfs ip=10.1.1.123:10.1.1.119:10.1.1.1:255.255.255.0::eth0:no console=ttySAC0,115200 console=fb";

	params->hdr.tag = ATAG_CMDLINE;
	params->hdr.size =
		(sizeof (struct tag_header) + strlen (commandline) + 1 + 3) >> 2;

	strcpy (params->u.cmdline.cmdline, commandline);

	params = tag_next (params);
}

static void setup_end_tag (void)
{
	params->hdr.tag = ATAG_NONE;
	params->hdr.size = 0;
}

void set_params(void)
{
	setup_start_tag();
	setup_memory_tags();
	setup_commandline_tag();
	setup_end_tag();
	
}


