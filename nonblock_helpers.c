#include <stdlib.h>

#include "wrappers.h"
#include "nonblock_helpers.h"

int put_until_emptyline (int fd, line_putter_t putter, void *data)
{
	char buf [20];
	ssize_t cnt;

	char *linebuf       = NULL;
	size_t linebuf_size = 0;
	size_t line_size    = 0;

	char c;
	int i;

	fd_set rset;

	for (;;){
		FD_ZERO (&rset);
		FD_SET (fd, &rset);

//		fprintf (stderr, "select\n");
		xselect (fd+1, &rset, NULL, NULL, NULL);

//		fprintf (stderr, "check\n");
		if (FD_ISSET (fd, &rset)){
//			fprintf (stderr, "yes\n");
			cnt = xread (fd, buf, sizeof (buf));

			if (line_size + cnt >= linebuf_size){
				linebuf = xrealloc (linebuf, linebuf_size + cnt+1);
				linebuf_size += cnt+1;
			}

//			fprintf (stderr, "cnt = %d\n", cnt);
			if (!cnt){
				linebuf [line_size] = 0;
				putter (linebuf, data);
				return 1;
			}

			for (i=0; i < cnt; ++i){
				c = buf [i];
				if (c != '\n'){
					linebuf [line_size++] = c;
				}else if (!line_size){
					return 0;
				}else{
					linebuf [line_size] = 0;
					putter (linebuf, data);
					line_size = 0;
				}
			}
		}
	}

	abort (); /* this should not happen */
	return 1;
}