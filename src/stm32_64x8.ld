ENTRY(Reset_Handler)

GROUP(libgcc.a libc.a libm.a)

MEMORY
{
	rom(RX) : ORIGIN = 0x08000000, LENGTH = 64k
	ram(RW) : ORIGIN = 0x20000000, LENGTH =  8k
}

PROVIDE(_estack = ORIGIN(ram) + LENGTH(ram));

SECTIONS
{
	.text : 
	{
		KEEP(*(.isr_vector))
		*(.text .text.*)      /* code */
		*(.rodata .rodata.*)  /* constants */
		. = ALIGN(4);
	} > rom
	
	.data : 
	{
		_sidata = LOADADDR(.data);
		_sdata = .;
		*(.data .data.*)
		. = ALIGN(4);
		_edata = .;
	} > ram AT > rom
	
	.bss : 
	{ 
		_sbss = .;
		*(.bss .bss.*) 
		*(COMMON)
		. = ALIGN(4);
		_ebss = .;
	} > ram
	
}