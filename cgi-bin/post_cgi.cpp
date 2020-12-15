#include<stdio.h>
#include<stdlib.h>

int main(int argc, char **argv, char **env)
{
	printf("Content-Type : text/html\r\n\r\n"); //http header : MIME type
  printf("<html>\n");
  printf("<body>\n");
  printf("<h1>Hello CGI World!</h1>\n");
  printf("<h2>Args Variables</h2>\n\n");
  int i = 0;
  while (i < argc) {
    printf("%s<br>\n", argv[i]);
    i++;
  }
  printf("</body>\n");
  printf("</html>\n");
  printf("<h2>Env Variables</h2>\n\n");
  i = 0;
  while (env[i]) {
    printf("%s<br>\n", env[i]);
    i++;
  }
  printf("</body>\n");
  printf("</html>\n");

	// int len;
	// char* lenstr = getenv("CONTENT_LENGTH");

	// if (lenstr != NULL && (len = atoi(lenstr)) != 0) {

	// 	char *post_data = new char[len];
	// 	fgets(post_data, len + 1, stdin);
	// 	printf("%d\n%s\n", len, post_data);
	// }

	return 0;
}