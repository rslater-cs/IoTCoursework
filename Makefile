CONTIKI_PROJECT = coursework 
all: $(CONTIKI_PROJECT) 
#UIP_CONF_IPV6=1 
CONTIKI = ../..
include $(CONTIKI)/Makefile.include
