#include "phos.h"
#include "hardware.h"

static void adc_task(int dummy) {
    int client, chan, result;
    message m;

    // Initialise the ADC: compare 1/3 of the input with 1/3 of Vdd
    ADC_CONFIG = FIELD(ADC_CONFIG_RES, ADC_CONFIG_RES_10bit)
        | FIELD(ADC_CONFIG_INPSEL, ADC_CONFIG_INPSEL_AIn_1_3)
        | FIELD(ADC_CONFIG_REFSEL, ADC_CONFIG_REFSEL_Vdd_1_3);
    ADC_ENABLE = 1;

    ADC_INTEN = BIT(ADC_INT_END);
    connect(ADC_IRQ);
    
    while (1) {
        receive(ANY, &m);
        assert(m.m_type == REQUEST);
        client = m.m_sender;
        chan = m.m_i1;

        SET_FIELD(ADC_CONFIG, ADC_CONFIG_PSEL, BIT(chan));
        ADC_START = 1;
        receive(HARDWARE, &m);
        assert(ADC_END);
        result = ADC_RESULT;
        SET_FIELD(ADC_CONFIG, ADC_CONFIG_PSEL, 0);
        ADC_END = 0;
        reconnect(ADC_IRQ);
        
        m.m_type = OK;
        m.m_i1 = result;
        send(client, &m);
    }
}

int adc_reading(int chan) {
    message m;
    m.m_type = REQUEST;
    m.m_i1 = chan;
    sendrec(ADC, &m);
    assert(m.m_type == OK);
    return m.m_i1;
}

void adc_init(void) {
    start(ADC, "Adc", adc_task, 0, 256);
}
