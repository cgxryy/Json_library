/*
  Copyright (c) 2009 Dave Gamble
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
 
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
 
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "cgxr_json.h"

/* Parse text to JSON, then render back to text, and print! */
void doit(char *text)
{
	char *out;
	cjson *json;
	
	json=json_Parse(text);
	out = json_Print(json);
	json_Delete(json);
	printf("%s\n", out);
/*	if (!json) {printf("Error before: [%s]\n",cJSON_GetErrorPtr());}
	else
	{
		out=cJSON_Print(json);
		cJSON_Delete(json);
		printf("%s\n",out);
		free(out);
	}
*/	
}

/* Read a file, parse, render back, etc. */
void dofile(char *filename)
{
	FILE *f=fopen(filename,"rb");fseek(f,0,SEEK_END);long len=ftell(f);fseek(f,0,SEEK_SET);
	char *data=(char*)malloc(len+1);fread(data,1,len,f);fclose(f);
	doit(data);
	free(data);
}


int main (int argc, const char * argv[]) {
	/* a bunch of json: */
//	char text1[]="{\n\"name\": \"Jack (\\\"Bee\\\") Nimble\", \n\"format\": {\"type\":       \"rect\", \n\"width\":      1920, \n\"height\":     1080, \n\"interlace\":  false,\"frame rate\": 24\n}\n}";	
//	char text2[]="[\"Sunday\", \"Monday\", \"Tuesday\", \"Wednesday\", \"Thursday\", \"Friday\", \"Saturday\"]";
//	char text3[]="[\n    [0, -1, 0],\n    [1, 0, 0],\n    [0, 0, 1]\n	]\n";
//	char text4[]="{\n		\"Image\": {\n			\"Width\":  800,\n			\"Height\": 600,\n			\"Title\":  \"View from 15th Floor\",\n			\"Thumbnail\": {\n				\"Url\":    \"http:/*www.example.com/image/481989943\",\n				\"Height\": 125,\n				\"Width\":  \"100\"\n			},\n			\"IDs\": [116, 943, 234, 38793]\n		}\n	}";
//	char text5[]="[\n	 {\n	 \"precision\": \"zip\",\n	 \"Latitude\":  37.7668,\n	 \"Longitude\": -122.3959,\n	 \"Address\":   \"\",\n	 \"City\":      \"SAN FRANCISCO\",\n	 \"State\":     \"CA\",\n	 \"Zip\":       \"94107\",\n	 \"Country\":   \"US\"\n	 },\n	 {\n	 \"precision\": \"zip\",\n	 \"Latitude\":  37.371991,\n	 \"Longitude\": -122.026020,\n	 \"Address\":   \"\",\n	 \"City\":      \"SUNNYVALE\",\n	 \"State\":     \"CA\",\n	 \"Zip\":       \"94085\",\n	 \"Country\":   \"US\"\n	 }\n	 ]";

	/* Process each json textblock by parsing, then rebuilding: */
/*	doit(text1);
	doit(text2);	
	doit(text3);
	doit(text4);
	doit(text5);
*/
	/* Parse standard testfiles: */
	dofile("t/test5"); 
//	dofile("t/test2"); 
//	dofile("t/test3"); 
/*	dofile("../../tests/test2"); 
	dofile("../../tests/test3"); 
	dofile("../../tests/test4"); 
	dofile("../../tests/test5"); 
*/
	/* Now some samplecode for building objects concisely: */
//	create_objects();
	
	return 0;
}
