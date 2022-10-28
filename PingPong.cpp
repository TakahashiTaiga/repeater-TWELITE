// use twelite mwx c++ template library
#include <TWELITE>
#include <NWK_SIMPLE>


#define BINARY_USE
#define BUF_SZ 64
#define HEADER1 0xA5
#define HEADER2 0x5A
#define HEADER3 0x80

struct  COM_TYP{
    uint8_t buf[BUF_SZ];
};

struct TXCOM_TYP{
    uint8_t header[3];
    uint8_t sz;
    uint8_t buf[BUF_SZ];
    uint8_t sum;
};

// application use
COM_TYP rcvdata;
TXCOM_TYP txdata;
uint8_t sbuf[]="TWE-CMD0-OK";
uint8_t sbuf1[]="TWE-CMD1-OK";

/*** Config part */
// application ID
const uint32_t APP_ID = 0x1234abcd;

// channel
const uint8_t CHANNEL = 13;

// DIO pins
const uint8_t PIN_BTN = 12;

/*** application defs */
// packet message
const int MSG_LEN = 10;

/*** setup procedure (run once at cold bootN) */
void setup() {
	/*** SETUP section */
	Analogue.setup(true); // setup analogue read (check every 50ms)

	// the twelite main class
	the_twelite
		<< TWENET::appid(APP_ID)    // set application ID (identify network group)
		<< TWENET::channel(CHANNEL) // set channel (pysical channel)
		<< TWENET::rx_when_idle();  // open receive circuit (if not set, it can't listen packts from others)

	// Register Network
	auto&& nwksmpl = the_twelite.network.use<NWK_SIMPLE>();
	nwksmpl << NWK_SIMPLE::logical_id(0xFE) // set Logical ID. (0xFE means a child device with no ID)
	        << NWK_SIMPLE::repeat_max(3);   // can repeat a packet up to three times. (being kind of a router)

	// initialize the object. (allocate Tx/Rx buffer, and etc..)
    // Serial.setup(64, 192);

	/*** BEGIN section */
	Analogue.begin(pack_bits(PIN_ANALOGUE::A1, PIN_ANALOGUE::VCC), 50); // _start continuous adc capture.

	the_twelite.begin(); // start twelite!

	// start the peripheral with 115200bps.
	//Serial.setup(64, 192);
    Serial.begin(9600);
	// SerialParser.begin(PARSER::BINARY,128); //バイナリ形式を選択　128バイト分の領域を確保
}

/*** loop procedure (called every event) */
void loop() {
	/*
	// Serial << "start" << mwx::crlf << mwx::flush;
	const char * msg = "testtest1";
	Serial << '\n' << msg << mwx::crlf << mwx::flush;;
	*/
	
	// receive RF packet.
    while (the_twelite.receiver.available()) {
		auto&& rx = the_twelite.receiver.read();

		uint8_t msg[MSG_LEN];
		
		uint16_t adcval, volt;
		uint32_t timestamp;

		// expand packet payload (shall match with sent packet data structure, see pack_bytes())
		expand_bytes(rx.get_payload().begin(), rx.get_payload().end()
					, msg       // 4bytes of msg
								//   also can be -> std::make_pair(&msg[0], MSG_LEN)
					, adcval    // 2bytes, A1 value [0..1023]
				    , volt      // 2bytes, Module VCC[mV]
					, timestamp // 4bytes of timestamp
        );
		
		// Serial << '\n' << "parentID:parent01, " << "childID:" << msg << mwx::crlf;
		// const char * sending_data = msg;
		Serial << '\n' << msg << mwx::crlf << mwx::flush;
		
		//Serial << '\n' << msg << mwx::crlf;
	}
	
}