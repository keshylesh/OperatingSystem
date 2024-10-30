#define VGA_ADDRESS 0xb8000
#define VGA_H 25
#define VGA_W 80
#define KB_DATA_PORT 0x60
#define ENTER 0x1c
#define BACKSPACE 0x0e


void clr_scn(char* vidptr, int color);
void print_scn(char* vidptr, int row, int col, int color, char* str);
void read_scn(char* vidptr, int row, int col, int color, char* buffer, int max_len);
unsigned char port_byte_in(unsigned short port);
char code_to_char(unsigned char code);

void kmain(void)
{
	char* str = "Welcome to NullOS!";
	char* vidptr = (char*) VGA_ADDRESS;
	char buffer[100];

	clr_scn(vidptr, 0x07);
	print_scn(vidptr, 0, 0, 0x02, str);

	print_scn(vidptr, 2, 0, 0x02, "Enter your username: ");

	read_scn(vidptr, 3, 0, 0x02, buffer, 100);

	print_scn(vidptr, 4, 0, 0x02, "You entered: ");
	print_scn(vidptr, 4, 13, 0x04, buffer);

	return;
}

void print_scn(char* vidptr, int row, int col, int color, char* str)
{
	unsigned int i = 0;
	unsigned int pos = (row * VGA_W + col) * 2;

	while (str[i] != '\0')
	{
		vidptr[pos] = str[i];
		vidptr[pos+1] = color;
		++i;
		pos += 2;

		if ((pos /  2) % VGA_W == 0)
		{
			row++;
			pos = row * VGA_W * 2;
		}
	}
}

void clr_scn(char* vidptr, int color)
{
	unsigned int i = 0;
	unsigned int j = 0;
	
	while (j < 80 * 25 * 2) // 25 lines 80 columns 2 bytes each
	{
		vidptr[j] = ' ';
		vidptr[j+1] = color;
		j += 2;
	}

	j = 0;
}

void read_scn(char* vidptr, int row, int col, int color, char* buffer, int max_len)
{
	unsigned int i = 0;
	unsigned char code;
	char c;

	while (i < max_len - 1)
	{
		code = port_byte_in(KB_DATA_PORT);

		if (code & 0x80)
			continue;

		if (code == ENTER)
		{
			buffer[i] = '\0';
			break;
		}

		if (code == BACKSPACE)
		{
			if (i > 0) 
			{
				i--;          
            			col--;         

           			print_scn(vidptr, row, col, color, '\0');
			}

           		continue;
		}

		c = code_to_char(code);

		if (c)
		{
			buffer[i++] = c;
			char temp[2] = { c, '\0' };
			print_scn(vidptr, row, col++, color, temp);
		}

		while ((port_byte_in(KB_DATA_PORT) & 0x80) == 0);
	}

	buffer[i] = '\0';
}

unsigned char port_byte_in(unsigned short port)
{
	unsigned char res;
	__asm__ volatile ("inb %1, %0" : "=a"(res) : "Nd"(port));
	return res;
}

char code_to_char(unsigned char code)
{
    char ascii[128] = {
        0,  27, '1', '2', '3', '4', '5', '6', '7', '8', /* 9 */
        '9', '0', '-', '=', '\b', /* Backspace (0x0E should return 0 or be ignored) */
        '\t',   /* Tab */
        'q', 'w', 'e', 'r', /* 19 */
        't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
        0,      /* Control (CTRL) key */
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
        '\'', '`',   0,    /* Left shift */
        '\\', 'z', 'x', 'c', 'v', 'b', 'n',      /* 49 */
        'm', ',', '.', '/',   0,    /* Right shift */
        '*',  0,  /* Alt */
        ' ',  /* Space bar */
        0,  /* Caps lock */
        /* Other keys can be added here if needed */
    };

    // Ignore special keys like backspace (0x0E) or others, return 0
    if (code == BACKSPACE) {
        return 0;  // Return nothing for backspace (do not convert it to any character)
    }

    // Return the corresponding ASCII character if valid
    return ascii[code];
}
