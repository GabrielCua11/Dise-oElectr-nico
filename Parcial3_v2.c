// VERSIÓN DONDE SE MUESTRA LA OPERACIÓN EN CONSOLA

#include <stdint.h>

// PERIPHERALS AND GPIO PORTS
#define PERIPHERAL_BASE_ADDRESS     0x40000000U
#define AHB_BASE_ADDRESS            (PERIPHERAL_BASE_ADDRESS + 0x00020000U)
#define APB1_BASE_ADDRESS			(PERIPHERAL_BASE_ADDRESS + 0x00000000U)
#define RCC_BASE_ADDRESS            (AHB_BASE_ADDRESS + 0x00001000U)
#define RCC_IOPENR_ADDRESS          (RCC_BASE_ADDRESS + 0x0000002CU)
#define IOPORT_ADDRESS              (PERIPHERAL_BASE_ADDRESS + 0x10000000U)
#define GPIOA_BASE_ADDRESS          (IOPORT_ADDRESS + 0x00000000U)
#define GPIOB_BASE_ADDRESS          (IOPORT_ADDRESS + 0x00000400U)
#define GPIOC_BASE_ADDRESS          (IOPORT_ADDRESS + 0x00000800U)

// USART2 BASE ADDRESS
#define USART2_BASE_ADDRESS			(APB1_BASE_ADDRESS + 0x00004400U)

// POINTERS GPIO AND RCC STRUCTURES
#define GPIOA 	((GPIO_RegDef_t*)GPIOA_BASE_ADDRESS)
#define GPIOB 	((GPIO_RegDef_t*)GPIOB_BASE_ADDRESS)
#define GPIOC 	((GPIO_RegDef_t*)GPIOC_BASE_ADDRESS)
#define RCC 	((RCC_RegDef_t*)RCC_BASE_ADDRESS)
#define USART2 	((USART_RegDef_t*)USART2_BASE_ADDRESS)

// STRUCTS
typedef struct
{
    uint32_t MODER;
    uint32_t OTYPER;
    uint32_t OSPEEDR;
    uint32_t PUPDR;
    uint32_t IDR;
    uint32_t ODR;
    uint32_t BSRR;
    uint32_t LCKR;
    uint32_t AFR[2];
    uint32_t BRR;
} GPIO_RegDef_t;

typedef struct
{
    uint32_t CR;
    uint32_t ICSCR;
    uint32_t CRRCR;
    uint32_t CFGR;
    uint32_t CIER;
    uint32_t CIFR;
    uint32_t CICR;
    uint32_t IOPRSTR;
    uint32_t AHBRSTR;
    uint32_t APB2RSTR;
    uint32_t APB1RSTR;
    uint32_t IOPENR;
    uint32_t AHBENR;
    uint32_t APB2ENR;
    uint32_t APB1ENR;
    uint32_t IOPSMENR;
    uint32_t AHBSMENR;
    uint32_t APB2SMENR;
    uint32_t APB1SMENR;
    uint32_t CCIPR;
    uint32_t CSR;
} RCC_RegDef_t;

typedef struct
{
	uint32_t CR1;
	uint32_t CR2;
	uint32_t CR3;
	uint32_t BRR;
	uint32_t GTPR;
	uint32_t RTOR;
	uint32_t RQR;
	uint32_t ISR;
	uint32_t ICR;
	uint32_t RDR;
	uint32_t TDR;
} USART_RegDef_t;

// 7-SEGMENT DISPLAYS CONSTANTS
#define cero  	0x3F
#define uno   	0x06
#define dos   	0x5B
#define tres  	0x4F
#define cuatro  0x66
#define cinco   0x6D
#define seis   	0x7D
#define siete  	0x07
#define ocho   	0x7F
#define nueve  	0x67
#define A 	   	0x77
#define B 	   	0x7C
#define C 	   	0x39
#define D 	   	0x4E
#define E 	   	0x79
#define F 	   	0x71

#define KEY_A_INDEX 3
#define KEY_B_INDEX 7
#define KEY_C_INDEX 11
#define KEY_D_INDEX 15
#define KEY_E_INDEX 12
#define KEY_F_INDEX 14

// FUNCTIONS
void delay_ms(uint16_t n);
void update_display_values(void);
void decimal_decoder(uint8_t value);
void USART2_write(uint8_t ch);
void USART2_text(uint8_t* StringPtr);
void USART2_send_int(int num);
void send_calculation(int sum_value, int stored_value, int operation);
uint8_t USART2_read(void);
uint8_t decoder(uint8_t value_to_decode);


// VARIABLES FOR DISPLAYS AND KEYS
int8_t	key_pressed = 0x00;
uint16_t first_digit_display = 0xFF;
uint16_t second_digit_display = 0xFF;
uint16_t third_digit_display = 0xFF;
uint16_t fourth_digit_display = 0xFF;
uint16_t fifth_digit_display = 0xFF;
uint16_t sixth_digit_display = 0xFF;
uint16_t seventh_digit_display = 0xFF;
uint16_t eighth_digit_display = 0xFF;
uint32_t stored_value = 0;
uint8_t mychar = 0x00;

// KEYPAD KEYS TO DISPLAY VALUES MAPPING
const uint8_t key_to_display[] = {
    uno, dos, tres, A,
    cuatro, cinco, seis, B,
    siete, ocho, nueve, C,
    E, cero, F, D
};

// GET NUMERICAL VALUE FROM KEYPAD KEY
uint8_t get_number_from_key(uint8_t key){

	switch (key){
		case 0: return 1;
		case 1: return 2;
		case 2: return 3;
		case 4: return 4;
		case 5: return 5;
		case 6: return 6;
		case 8: return 7;
		case 9: return 8;
		case 10: return 9;
		case 13: return 0;
		default: return 0;
	}
}

// GET KEYPAD KEY FROM NUMERICAL VALUE
uint8_t get_key_from_number(uint8_t number){

	switch (number){
		case 1: return 0;
		case 2: return 1;
		case 3: return 2;
		case 4: return 4;
		case 5: return 5;
		case 6: return 6;
		case 7: return 8;
		case 8: return 9;
		case 9: return 10;
		case 0: return 13;
		default: return 13;
	}
}

// SQUARE ROOT FUNCTION
uint32_t int_sqrt(uint32_t value) {

    uint32_t root = 0;
    uint32_t bit = 1 << 30; // The second-to-top bit is set

    // "bit" starts at the highest power of four <= the argument.
    while (bit > value) {
        bit >>= 2;
    }

    while (bit != 0) {
        if (value >= root + bit) {
            value -= root + bit;
            root = (root >> 1) + bit;
        } else {
            root = root >> 1;
        }
        bit >>= 2;
    }
    return (uint32_t)root;
}

int main(void) {

	// ENABLE CLOCKS FOR GPIO A, B & C
    RCC->IOPENR |= (1 << 2) | (1 << 1) | (1 << 0);

    // CONFIGURE GPIO C PINS (4 TO 11) FOR DISPLAYS
    GPIOC->MODER &= ~(0b11 << 8 | 0b11 << 10 | 0b11 << 12 | 0b11 << 14 | 0b11 << 16 | 0b11 << 18 | 0b11 << 20 | 0b11 << 22);
    GPIOC->MODER |= (0b01 << 8) | (0b01 << 10) | (0b01 << 12) | (0b01 << 14) | (0b01 << 16) | (0b01 << 18) | (0b01 << 20) | (0b01 << 22);

    // CONFIGURE GPIO B PINS (0 TO 7) FOR SEGMENTS
    GPIOB->MODER &= ~(0xFFFF);
    GPIOB->MODER |= (0b01 << 0) | (0b01 << 2) | (0b01 << 4) | (0b01 << 6) | (0b01 << 8) | (0b01 << 10) | (0b01 << 12) | (0b01 << 14);

    // CONFIGURE GPIO B PINS (8 TO 11) FOR KEYPAD ROWS
    GPIOB->PUPDR &= ~((0b11 << 16) | (0b11 << 18) | (0b11 << 20) | (0b11 << 22));
    GPIOB->PUPDR |= ((0b01 << 16) | (0b01 << 18) | (0b01 << 20) | (0b01 << 22));
    GPIOB->MODER &= ~((0b11 << 16) | (0b11 << 18) | (0b11 << 20) | (0b11 << 22));

    // CONFIGURE GPIO B PINS (12 TO 15) FOR KEYPAD COLUMNS
    GPIOB->MODER &= ~((0b11 << 24) | (0b11 << 26) | (0b11 << 28) | (0b11 << 30));
    GPIOB->MODER |= ((0b01 << 24) | (0b01 << 26) | (0b01 << 28) | (0b01 << 30));

    // USART2 PIN CONFIGURATION
    	// Alternate function mode for PA2 and PA3
    GPIOA->MODER &= ~(1<<4);
    GPIOA->MODER &= ~(1<<6);
    	// Select which MODER, AF4 for USART2, TX on PA2 :: RX on PA3
    GPIOA->AFR[0] |= 1<<10;
    GPIOA->AFR[0] |= 1<<14;

    // USART2 PERIPHERAL CONFIGURATION
    	// USART2 Clock Enable
    RCC->APB1ENR |= 1<<17;
    	// Baud Rate Calculation
    USART2->BRR = 218;
    	// Enable TX and RX
    USART2->CR1 |= (1<<2)|(1<<3);
    	// Enable Peripheral
    USART2->CR1 |= 1<<0;

    while (1) {

        for (int col = 0; col < 4; col++) {

        	// Set all column lines to high and then set the current column to low
            GPIOB->ODR = (0xF << 12);
            GPIOB->ODR &= ~(1 << (12 + col));

            delay_ms(10);

            // Read the rows
            uint32_t rows = GPIOB->IDR & (0xF << 8);

            for (int row = 0; row < 4; row++) {

            	// Check if a key is pressed
                if (!(rows & (1 << (row + 8)))) {

                	// Calculate the key number based on row and column
                    uint8_t key_number = col * 4 + row;

                	int sum_value = 0;
                	int u_value = get_number_from_key(first_digit_display);
                	int d_value = get_number_from_key(second_digit_display);
                	int c_value = get_number_from_key(third_digit_display);
                	int um_value = get_number_from_key(fourth_digit_display);
                	int dm_value = get_number_from_key(fifth_digit_display);
                	int cm_value = get_number_from_key(sixth_digit_display);
                	int m_value = get_number_from_key(seventh_digit_display);
                	int mm_value = get_number_from_key(eighth_digit_display);

                    // Key F: Store the value
                    if (key_number == KEY_F_INDEX) {

                       	sum_value =  u_value + (d_value * 10) + (c_value * 100) + (um_value * 1000)  + (dm_value * 10000) + (cm_value * 100000) + (m_value * 1000000) + (mm_value * 10000000);

						stored_value = (uint32_t)(stored_value + sum_value);

                        first_digit_display = second_digit_display = third_digit_display = fourth_digit_display = fifth_digit_display = sixth_digit_display = seventh_digit_display = eighth_digit_display = 0xff;
                    }

                	// Key A: Sum the displayed value to the stored value
                    else if (key_number == KEY_A_INDEX) {

                        if (stored_value != 0) {

                        	sum_value =  sum_value +  u_value + (d_value * 10) + (c_value * 100) + (um_value * 1000)  + (dm_value * 10000) + (cm_value * 100000) + (m_value * 1000000) + (mm_value * 10000000);

                        	stored_value = (uint32_t)(stored_value + sum_value);

                        	u_value = stored_value % 10;
                        	d_value = ((stored_value - u_value) % 100) / 10;
                        	c_value = ((stored_value - d_value - u_value) % 1000) / 100;
                        	um_value = ((stored_value - c_value - d_value - u_value) % 10000) / 1000;
                        	dm_value = ((stored_value - um_value - c_value - d_value - u_value) % 100000) / 10000;
                        	cm_value = ((stored_value - dm_value - um_value - c_value - d_value - u_value) % 1000000) / 100000;
                        	m_value = ((stored_value - cm_value - dm_value - um_value - c_value - d_value - u_value) % 10000000) / 1000000;
                        	mm_value = ((stored_value - m_value - cm_value - dm_value - um_value - c_value - d_value - u_value) % 100000000) / 10000000;

                        	first_digit_display =  get_key_from_number(u_value);
                        	second_digit_display = get_key_from_number(d_value);
                        	third_digit_display =  get_key_from_number(c_value);
                        	fourth_digit_display = get_key_from_number(um_value);
                        	fifth_digit_display =  get_key_from_number(dm_value);
							sixth_digit_display = get_key_from_number(cm_value);
							seventh_digit_display =  get_key_from_number(m_value);
							eighth_digit_display = get_key_from_number(mm_value);

							send_calculation(sum_value, stored_value, 1);

                        	stored_value = 0;
                        }
                    }

                	// Key B: Subtract the displayed value from the stored value
                    else if (key_number == KEY_B_INDEX) {

                        if (stored_value != 0) {

                        	sum_value =  sum_value +  u_value + (d_value * 10) + (c_value * 100) + (um_value * 1000)  + (dm_value * 10000) + (cm_value * 100000) + (m_value * 1000000) + (mm_value * 10000000);

                        	stored_value = (uint32_t)(stored_value - sum_value);

                        	u_value = stored_value % 10;
                        	d_value = ((stored_value - u_value) % 100) / 10;
                        	c_value = ((stored_value - d_value - u_value) % 1000) / 100;
                        	um_value = ((stored_value - c_value - d_value - u_value) % 10000) / 1000;
							dm_value = ((stored_value - um_value - c_value - d_value - u_value) % 100000) / 10000;
							cm_value = ((stored_value - dm_value - um_value - c_value - d_value - u_value) % 1000000) / 100000;
							m_value = ((stored_value - cm_value - dm_value - um_value - c_value - d_value - u_value) % 10000000) / 1000000;
							mm_value = ((stored_value - m_value - cm_value - dm_value - um_value - c_value - d_value - u_value) % 100000000) / 10000000;

                        	first_digit_display =  get_key_from_number(u_value);
                        	second_digit_display = get_key_from_number(d_value);
                        	third_digit_display =  get_key_from_number(c_value);
                        	fourth_digit_display = get_key_from_number(um_value);
                        	fifth_digit_display =  get_key_from_number(dm_value);
							sixth_digit_display = get_key_from_number(cm_value);
							seventh_digit_display =  get_key_from_number(m_value);
							eighth_digit_display = get_key_from_number(mm_value);

							send_calculation(sum_value, stored_value, 2);

                        	stored_value = 0;
                        }
                    }

                    // Key C: Multiply the stored value by the displayed value
                    else if (key_number == KEY_C_INDEX) {

                        if (stored_value != 0) {

                        	sum_value =  sum_value +  u_value + (d_value * 10) + (c_value * 100) + (um_value * 1000)  + (dm_value * 10000) + (cm_value * 100000) + (m_value * 1000000) + (mm_value * 10000000);

                        	stored_value = (uint32_t)(stored_value * sum_value);

                        	u_value = stored_value % 10;
							d_value = ((stored_value - u_value) % 100) / 10;
							c_value = ((stored_value - d_value - u_value) % 1000) / 100;
							um_value = ((stored_value - c_value - d_value - u_value) % 10000) / 1000;
							dm_value = ((stored_value - um_value - c_value - d_value - u_value) % 100000) / 10000;
							cm_value = ((stored_value - dm_value - um_value - c_value - d_value - u_value) % 1000000) / 100000;
							m_value = ((stored_value - cm_value - dm_value - um_value - c_value - d_value - u_value) % 10000000) / 1000000;
							mm_value = ((stored_value - m_value - cm_value - dm_value - um_value - c_value - d_value - u_value) % 100000000) / 10000000;

							first_digit_display = get_key_from_number(u_value);
							second_digit_display = get_key_from_number(d_value);
							third_digit_display = get_key_from_number(c_value);
							fourth_digit_display = get_key_from_number(um_value);
							fifth_digit_display = get_key_from_number(dm_value);
							sixth_digit_display = get_key_from_number(cm_value);
							seventh_digit_display = get_key_from_number(m_value);
							eighth_digit_display = get_key_from_number(mm_value);

							send_calculation(sum_value, stored_value, 3);

                        	stored_value = 0;
                        }
                    }

                    // Key D: Divide the stored value by the displayed value
                    else if (key_number == KEY_D_INDEX) {

                        int div_value = 0;

                        div_value = u_value + (d_value * 10) + (c_value * 100) + (um_value * 1000)  + (dm_value * 10000) + (cm_value * 100000) + (m_value * 1000000) + (mm_value * 10000000);

                        if (stored_value != 0 && div_value != 0) {

                            stored_value = (uint32_t)(stored_value / div_value);

                            u_value = stored_value % 10;
                            d_value = (stored_value / 10) % 10;
                            c_value = (stored_value / 100) % 10;
                            um_value = (stored_value / 1000) % 10;
                            dm_value = (stored_value / 10000) % 10;
							cm_value = (stored_value / 100000) % 10;
							m_value = (stored_value / 1000000) % 10;
							mm_value = (stored_value / 10000000) % 10;

                            first_digit_display = get_key_from_number(u_value);
                            second_digit_display = get_key_from_number(d_value);
                            third_digit_display = get_key_from_number(c_value);
                            fourth_digit_display = get_key_from_number(um_value);
							fifth_digit_display =  get_key_from_number(dm_value);
							sixth_digit_display = get_key_from_number(cm_value);
							seventh_digit_display =  get_key_from_number(m_value);
							eighth_digit_display = get_key_from_number(mm_value);

							send_calculation(div_value, stored_value, 4);

                            stored_value = 0;
                        }
                    }

                    else if (key_number == KEY_E_INDEX) {
                        // Calculate the square root of the displayed value
                        int displayed_value = 0;

                        displayed_value = u_value + (d_value * 10) + (c_value * 100) + (um_value * 1000)  + (dm_value * 10000) + (cm_value * 100000) + (m_value * 1000000) + (mm_value * 10000000);

                        uint32_t sqrt_value = int_sqrt(displayed_value);

                        // Convert the square root back to display format
                        u_value = sqrt_value % 10;
                        d_value = (sqrt_value / 10) % 10;
                        c_value = (sqrt_value / 100) % 10;
                        um_value = (sqrt_value / 1000) % 10;
                        dm_value = (sqrt_value / 10000) % 10;
						cm_value = (sqrt_value / 100000) % 10;
						m_value = (sqrt_value / 1000000) % 10;
						mm_value = (sqrt_value / 10000000) % 10;

                        first_digit_display = get_key_from_number(u_value);
                        second_digit_display = get_key_from_number(d_value);
                        third_digit_display = get_key_from_number(c_value);
                        fourth_digit_display = get_key_from_number(um_value);
						fifth_digit_display =  get_key_from_number(dm_value);
						sixth_digit_display = get_key_from_number(cm_value);
						seventh_digit_display =  get_key_from_number(m_value);
						eighth_digit_display = get_key_from_number(mm_value);

						send_calculation(displayed_value, sqrt_value, 5);
                    }

                    else {

                    	eighth_digit_display = seventh_digit_display;
                    	seventh_digit_display = sixth_digit_display;
                    	sixth_digit_display = fifth_digit_display;
                    	fifth_digit_display = fourth_digit_display;
                    	fourth_digit_display = third_digit_display;
                    	third_digit_display = second_digit_display;
                        second_digit_display = first_digit_display;
                        first_digit_display = key_number;
                    }

                    // Wait for the key to be released
                    while (!(GPIOB->IDR & (1 << (row + 8))));
                    break;
                }
            }

            // Reset the current column to high
            GPIOB->ODR |= (1 << (12 + col));
        }

        update_display_values();
        // Ensure the stored value does not exceed 999
        stored_value = stored_value %100000000;
    }
}

// UART TX
void USART2_write(uint8_t ch) {

	while(!(USART2->ISR & 0x0080)){}
	USART2->TDR = ch;
}

void USART2_text(uint8_t* StringPtr) {

	while(*StringPtr != 0x00) {

		USART2_write(*StringPtr);
		StringPtr++;
	}
}

uint8_t USART2_read(void) {

	while(!(USART2->ISR & 0x0020)){}
	return USART2->RDR;
}

void USART2_send_int(int num) {

    char buf[12];  // Buffer to hold the integer conversion, sufficiently large for an int
    sprintf(buf, "%d", num);  // Convert integer to string
    USART2_text((uint8_t*) buf);  // Send the string
}

void send_calculation(int sum_value, int stored_value, int operation) {

	if (operation == 1) {

	    USART2_send_int(stored_value - sum_value);
	    USART2_text(" + ");
	    USART2_send_int(sum_value);
	    USART2_text(" = ");
	    USART2_send_int(stored_value);
	}
	else if (operation == 2) {

	    USART2_send_int(stored_value + sum_value);
	    USART2_text(" - ");
	    USART2_send_int(sum_value);
	    USART2_text(" = ");
	    USART2_send_int(stored_value);

	}
	else if (operation == 3) {

	    USART2_send_int(stored_value / sum_value);
	    USART2_text(" * ");
	    USART2_send_int(sum_value);
	    USART2_text(" = ");
	    USART2_send_int(stored_value);

	}
	else if (operation == 4) {

	    USART2_send_int(stored_value * sum_value);
	    USART2_text(" / ");
	    USART2_send_int(sum_value);
	    USART2_text(" = ");
	    USART2_send_int(stored_value);

	}
	else if (operation == 5) {

	    USART2_text("√");
	    USART2_send_int(stored_value * stored_value);
	    USART2_text(" = ");
	    USART2_send_int(stored_value);

	}
	else {

		USART2_text("Error");
	}
    USART2_write(0x000D);
    USART2_write(0x000A);
}

// DELAY FUNCTION
void delay_ms(uint16_t n) {

    for (uint32_t i = 0; i < n * 10; i++) {
        __asm__("NOP");
    }
}

// UPDATE VALUES FUNCTION
void update_display_values(void) {

    if (eighth_digit_display != 0xFF) {

		GPIOC->BSRR = 1 << (6 + 16);
		GPIOC->BSRR = 1 << (5 + 16);
		GPIOC->BSRR = 1 << (8 + 16);
		GPIOC->BSRR = 1 << (9 + 16);
		GPIOC->BSRR = 1 << (7 + 16);
		GPIOC->BSRR = 1 << (4 + 16);
		GPIOC->BSRR = 1 << (10 + 16);
		GPIOC->BSRR = 1 << 11;
		GPIOB->BSRR = 0xFF << 16;
		GPIOB->BSRR = decoder(eighth_digit_display);
		delay_ms(5);
    }

    if (seventh_digit_display != 0xFF) {

		GPIOC->BSRR = 1 << (6 + 16);
		GPIOC->BSRR = 1 << (5 + 16);
		GPIOC->BSRR = 1 << (8 + 16);
		GPIOC->BSRR = 1 << (9 + 16);
		GPIOC->BSRR = 1 << (7 + 16);
		GPIOC->BSRR = 1 << (4 + 16);
		GPIOC->BSRR = 1 << (11 + 16);
		GPIOC->BSRR = 1 << 10;
		GPIOB->BSRR = 0xFF << 16;
		GPIOB->BSRR = decoder(seventh_digit_display);
		delay_ms(5);
    }

    if (sixth_digit_display != 0xFF) {

		GPIOC->BSRR = 1 << (6 + 16);
		GPIOC->BSRR = 1 << (5 + 16);
		GPIOC->BSRR = 1 << (8 + 16);
		GPIOC->BSRR = 1 << (9 + 16);
		GPIOC->BSRR = 1 << (7 + 16);
		GPIOC->BSRR = 1 << (10 + 16);
		GPIOC->BSRR = 1 << (11 + 16);
		GPIOC->BSRR = 1 << 4;
		GPIOB->BSRR = 0xFF << 16;
		GPIOB->BSRR = decoder(sixth_digit_display);
		delay_ms(5);
    }

    if (fifth_digit_display != 0xFF) {

		GPIOC->BSRR = 1 << (6 + 16);
		GPIOC->BSRR = 1 << (5 + 16);
		GPIOC->BSRR = 1 << (8 + 16);
		GPIOC->BSRR = 1 << (9 + 16);
		GPIOC->BSRR = 1 << (4 + 16);
		GPIOC->BSRR = 1 << (10 + 16);
		GPIOC->BSRR = 1 << (11 + 16);
		GPIOC->BSRR = 1 << 7;
		GPIOB->BSRR = 0xFF << 16;
		GPIOB->BSRR = decoder(fifth_digit_display);
		delay_ms(5);
    }

    if (fourth_digit_display != 0xFF) {

		GPIOC->BSRR = 1 << (6 + 16);
		GPIOC->BSRR = 1 << (5 + 16);
		GPIOC->BSRR = 1 << (8 + 16);
		GPIOC->BSRR = 1 << (4 + 16);
		GPIOC->BSRR = 1 << (7 + 16);
		GPIOC->BSRR = 1 << (10 + 16);
		GPIOC->BSRR = 1 << (11 + 16);
		GPIOC->BSRR = 1 << 9;
		GPIOB->BSRR = 0xFF << 16;
		GPIOB->BSRR = decoder(fourth_digit_display);
		delay_ms(5);
    }

    if (third_digit_display != 0xFF) {

		GPIOC->BSRR = 1 << (6 + 16);
		GPIOC->BSRR = 1 << (5 + 16);
		GPIOC->BSRR = 1 << (9 + 16);
		GPIOC->BSRR = 1 << (7 + 16);
		GPIOC->BSRR = 1 << (4 + 16);
		GPIOC->BSRR = 1 << (10 + 16);
		GPIOC->BSRR = 1 << (11 + 16);
		GPIOC->BSRR = 1 << 8;
		GPIOB->BSRR = 0xFF << 16;
		GPIOB->BSRR = decoder(third_digit_display);
		delay_ms(5);
    }

    if (second_digit_display != 0xFF) {
    	GPIOC->BSRR = 1 << (8 + 16);
        GPIOC->BSRR = 1 << (5 + 16);
        GPIOC->BSRR = 1 << (9 + 16);
        GPIOC->BSRR = 1 << (4 + 16);
        GPIOC->BSRR = 1 << (7 + 16);
        GPIOC->BSRR = 1 << (10 + 16);
        GPIOC->BSRR = 1 << (11 + 16);
        GPIOC->BSRR = 1 << 6;
        GPIOB->BSRR = 0xFF << 16;
        GPIOB->BSRR = decoder(second_digit_display);
        delay_ms(5);
    }

    if (first_digit_display != 0xFF) {
    	GPIOC->BSRR = 1 << (8 + 16);
        GPIOC->BSRR = 1 << (6 + 16);
        GPIOC->BSRR = 1 << (9 + 16);
        GPIOC->BSRR = 1 << (4 + 16);
        GPIOC->BSRR = 1 << (7 + 16);
        GPIOC->BSRR = 1 << (10 + 16);
        GPIOC->BSRR = 1 << (11 + 16);
        GPIOC->BSRR = 1 << 5;
        GPIOB->BSRR = 0xFF << 16;
        GPIOB->BSRR = decoder(first_digit_display);
        delay_ms(5);
    }
}

// Decode a number to its 7-segment display representation
uint8_t decoder(uint8_t number) {

	uint8_t key = 0;

	switch (number){

		case 0: key = 0; break;
		case 1: key = 1; break;
		case 2: key = 2; break;
		case 4: key = 4; break;
		case 5: key = 5; break;
		case 6: key = 6; break;
		case 8: key = 8; break;
		case 9: key = 9; break;
		case 10: key = 10; break;
		case 13: key = 13; break;
		default: return 13;
	}

	return key_to_display[key];
}

void decimal_decoder(uint8_t value) {

    if (value < 10) {

    	eighth_digit_display = seventh_digit_display;
    	seventh_digit_display = sixth_digit_display;
    	sixth_digit_display = fifth_digit_display;
    	fifth_digit_display = fourth_digit_display;
    	fourth_digit_display = third_digit_display;
    	third_digit_display = second_digit_display;
        second_digit_display = first_digit_display;
        first_digit_display = value;
    }
}
