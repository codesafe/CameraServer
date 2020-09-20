#pragma once

#define	PACKET_TRY_CONNECT		0x05	// connect to server
#define	PACKET_SHOT				0x10	// shot picture
#define PACKET_HALFPRESS		0x20	// auto focus

#define PACKET_ISO				0x31
#define PACKET_APERTURE			0x32
#define PACKET_SHUTTERSPEED		0x33

#define PACKET_FORCE_UPLOAD		0x40	// for test
#define PACKET_UPLOAD_PROGRESS	0x41
#define PACKET_UPLOAD_DONE		0x42
