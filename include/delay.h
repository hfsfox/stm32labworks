
void delay(uint32_t us)
{
	for(--ms)
	{
		__asm volatile
			(
			"nop"
			);
	}
}
