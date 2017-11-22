#include <iostream>
#include <math.h>

static inline unsigned __anint( double x) throw()
{
	return unsigned(floor(x+0.5));
}

inline static
void compress8to2()
{
	unsigned j=0;
	for(unsigned i=0;i<256;++i)
	{
		++j;
		fprintf(stderr,"%u",__anint(3.0*i/255));
		if(i<255) fprintf(stderr,", ");
		if(j>=16)
		{
			j=0;
			fprintf(stderr,"\n");
		}
	}
}

int main()
{
	
	std::cerr << "Hello!" << std::endl;
	compress8to2();
	return 0;
}

