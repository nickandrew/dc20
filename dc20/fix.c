#include <stdio.h>

#define min3(a, b, c) ((a < b) ? (a < c) ? a : c : (b < c) ? b : c)

main()
{
	unsigned char *r, *g, *b;
	int w = 0, h = 0;
	int sect[3][3][3];
	int c;
	int s;
	int i, j, k;

	memset((char *)sect, '\0', sizeof(sect));

	if (!read_header(&w, &h)) {
		fprintf(stderr, "Bad header.\n");
		exit(-1);
	}

	if ((r = (char *)malloc(w*h)) == NULL) {
		fprintf(stderr, "Could not allocate red array.\n");
		exit(-1);;
	}

	if ((g = (char *)malloc(w*h)) == NULL) {
		fprintf(stderr, "Could not allocate green array.\n");
		exit(-1);;
	}

	if ((b = (char *)malloc(w*h)) == NULL) {
		fprintf(stderr, "Could not allocate blue array.\n");
		exit(-1);;
	}

	fprintf(stderr, "\n");

	for (i = 0; i < w*h; i++) {

		if ((c = getchar()) == EOF) {
			fprintf(stderr, "Error reading red (%d).\n", i);
			exit(-1);
		}

		r[i] = c;

		if ((c = getchar()) == EOF) {
			fprintf(stderr, "Error reading green (%d, %d, %d).\n", i, w, h);
			exit(-1);
		}

		g[i] = c;

		if ((c = getchar()) == EOF) {
			fprintf(stderr, "Error reading blue (%d).\n", i);
			exit(-1);
		}

		b[i] = c;
		(sect[r[i]/86][g[i]/86][b[i]/86])++;
	}

	for (i = 0; i < 3; i++) {

		for (j = 0; j < 3; j++) {

			for (k = 0; k < 3; k++) {
				fprintf(stderr, "%5d", sect[i][j][k]);

				if (k < 2)
					fprintf(stderr, ", ");

			}

			fprintf(stderr, "\n");
		}

		fprintf(stderr, "\n");
	}

/*
	for (i = 0; i < w*h; i++) {
		r[i] = (r[i] > red) ? r[i] - red : 0;
		g[i] = (g[i] > green) ? g[i] - green : 0;
		b[i] = (b[i] > blue) ? b[i] - blue : 0;
	}
*/
	printf("P6\n%d %d\n255\n", w, h);

	for (i = 0; i < w*h; i++) {
		putchar(r[i]);
		putchar(g[i]);
		putchar(b[i]);
	}

}

read_header(w, h)
int *w;
int *h;
{
	int c;

	*w = *h = 0;

	if (getchar() != 'P' || getchar() != '6')
		return(0);

	while ((c = getchar()) != EOF) {

		if (c == '#') {

			while ((c = getchar()) != '\n' && c != EOF)
				;

			continue;
		}

		if (c != ' ' && c != '\n' && c != '\t') {
			ungetc(c, stdin);
			break;
		}

	}

	fprintf(stderr, "%d\n", scanf("%d %d %d", w, h, &c));
	fprintf(stderr, "%dx%d\n", *w, *h);
	return(1);
}
