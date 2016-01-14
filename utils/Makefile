# Meant to be built on a BeagleBone (not cross-compiled)

TWI_MAKEFILE  := twiboot_client/Makefile
TWI_MAKE_VARS := SRC_TREE=$(dir $(TWI_MAKEFILE))

all: ina219 power twiboot

ina219:	ina219.c
	gcc -o ina219 ina219.c

power:	powercape.c
	gcc -o power powercape.c

twiboot:
	$(MAKE) -f $(TWI_MAKEFILE) $(TWI_MAKE_VARS)

clean:
	rm -f power ina219
	$(MAKE) -f $(TWI_MAKEFILE) $(TWI_MAKE_VARS) clean
