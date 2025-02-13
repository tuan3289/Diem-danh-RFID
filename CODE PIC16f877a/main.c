#include <16F877A.h>
#device ADC=10
#FUSES NOWDT, NOBROWNOUT, NOLVP
#use delay(crystal=20MHz)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#use rtos(timer=0, minor_cycle=10ms)

#define LCD_ENABLE_PIN PIN_D2
#define LCD_RS_PIN PIN_D0
#define LCD_RW_PIN PIN_D1
#define LCD_DATA4 PIN_D4
#define LCD_DATA5 PIN_D5
#define LCD_DATA6 PIN_D6
#define LCD_DATA7 PIN_D7
#include <lcd.c>

#define MFRC522_CS PIN_E2
#define MFRC522_SCK PIN_C3
#define MFRC522_SI PIN_C4
#define MFRC522_SO PIN_C5
#define MFRC522_RST PIN_E0

#include <Built_in.h>

char employeeUID[3][6] = {
    {0x0E, 0xFE, 0x30, 0x02, 0xC2},
    {0x0E, 0xFE, 0x30, 0x02, 0x62},
    {0xA3, 0x2B, 0x14, 0x2D, 0xB1}
};

char employeeNames[3][20] = {
    "NGUYEN MANH TUAN-04",
    "BUI TRUNG CHINH-99",
    "VO THANH SANG-0324"
};

char UID[6];
unsigned int TagType;
int found = 0;
int sentFlag = 0;
int timeout_counter = 0;

int1 check_UID(char Data[], char UID[]) {
    for (int i = 0; i < 5; i++) {
        if (UID[i] != Data[i]) {
            return false;
        }
    }
    return true;
}

#task(rate=100ms, max=10ms)
void read_rfid() {
    if (MFRC522_isCard(&TagType)) {
        if (MFRC522_ReadCardSerial(&UID)) {
            found = 0;
            for (int j = 0; j < 3; j++) {
                if (check_UID(employeeUID[j], UID)) {
                    found = 1;
                    if (sentFlag == 0) {
                        printf("%s\n", employeeNames[j]);
                        sentFlag = 1;
                    }
                    break;
                }
            }
            if (found == 0 && sentFlag == 0) {
                for (int i = 0; i < 5; i++) {
                    printf("%X", UID[i]);
                }
                sentFlag = 1;
            }
            MFRC522_Halt();
            timeout_counter = 0; // Ð?t l?i bi?n ð?m th?i gian ch?
        }
    } else {
        sentFlag = 0;
        if (timeout_counter < 50) { // 50 x 100ms = 5 giây
            timeout_counter++;
        }
    }
}

#task(rate=200ms, max=10ms)
void update_lcd() {
    if (UID[0] != 0) { // Ki?m tra UID có giá tr? h?p l?
        if (found) {
            for (int j = 0; j < 3; j++) {
                if (check_UID(employeeUID[j], UID)) {
                    printf(LCD_PUTC, "\fTHANH CONG!!!!");
                    printf(LCD_PUTC, "\n%s", employeeNames[j]);
                    break;
                }
            }
        } else {
            printf(LCD_PUTC, "\fKHONG PHAI NV");
            printf(LCD_PUTC, "\nID: ");
            for (int i = 0; i < 5; i++) {
                printf(LCD_PUTC, "%X", UID[i]);
            }
        }
    }

    if (timeout_counter >= 50) { // N?u không có quét th? trong 5 giây
        printf(LCD_PUTC, "\fXIN CHAO!!!!");
        memset(UID, 0, sizeof(UID));
          printf(LCD_PUTC, "\nVUILONG QUETTHE");// Ð?t l?i UID
    }
}

void main() {
    lcd_init();
    printf(LCD_PUTC, "XIN CHAO!!!!!");
    delay_ms(1000);
    printf(LCD_PUTC, "\nVUILONG QUETTHE");
    MFRC522_Init();

    // Kh?i t?o các bi?n
    memset(UID, 0, sizeof(UID));
    TagType = 0;
    found = 0;
    sentFlag = 0;
    timeout_counter = 0;

    rtos_run();
}

