# AI-generated Makefile
CC=gcc
CFLAGS=-I$(HOME)/papi/include
LDFLAGS=-L$(HOME)/papi/lib
LIBS=-lpapi -lnvidia-ml

TARGET=read_papi_power

SRC=read_papi_power.c

$(TARGET): $(SRC)
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)