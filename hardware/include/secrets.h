#include <pgmspace.h>
#include <string.h>

#define SECRET
#define THINGNAME = ""

const char WIFI_SSID[] = "";
const char WIFI_PASSWORD[] = "";
const char AWS_IOT_ENDPOINT[] = "";

static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
... certyfikat AWS Root CA ...
-----END CERTIFICATE-----
)EOF";

// Device Certificate
static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
... certyfikat urządzenia ...
-----END CERTIFICATE-----
)KEY";

// Device Private Key
static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
... klucz prywatny ...
-----END RSA PRIVATE KEY-----
)KEY";