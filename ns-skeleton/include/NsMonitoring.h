/*
 * Copyright 2019 dmerkushov.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * File:   NsMonitoring.h
 * Author: dmerkushov
 *
 * Created on July 8, 2019, 2:56 PM
 */

#ifndef NSMONITORING_H
#define NSMONITORING_H

#include <string>
#include <sstream>
#include <vector>

#include <cstdint>
#include <cstring>

#include <byteswap.h>
#include <endian.h>

#include "NsSerializer.h"
#include "NsSkelConfiguration.h"

// Need to define UINT8_MAX and UINT16_MAX on Astra 1.5
#ifndef UINT8_MAX
#define UINT8_MAX (255)
#endif
#ifndef UINT16_MAX
#define UINT16_MAX (65535)
#endif

namespace nanoservices {

const std::string CONFIG_PARAMNAME_NSMONITORING_ENABLED ("ns-monitoring-enabled");
const std::string CONFIG_PARAMNAME_NSMONITORING_HOST ("ns-monitoring-host");
const std::string CONFIG_PARAMNAME_NSMONITORING_PORT_UDP ("ns-monitoring-port-udp");

const char EVENT_STARTUP_NOHTTP = 0x00;
const char EVENT_STARTUP_HTTP = 0x01;
const char EVENT_SENDRPCREQUEST = 0x10;
const char EVENT_SENDRPCREQUEST_ERROR = 0x11;
const char EVENT_RECEIVERPCRESPONSE = 0x20;
const char EVENT_RECEIVERPCRESPONSE_ERROR = 0x21;
const char EVENT_RECEIVERPCREQUEST = 0x30;
const char EVENT_RECEIVERPCREQUEST_ERROR = 0x31;
const char EVENT_SENDRPCRESPONSE = 0x40;
const char EVENT_SENDRPCRESPONSE_ERROR = 0x41;
const char EVENT_SHUTDOWN = 0xFF;

class NsMonIp4Address {
private:
	uint32_t _ipv4;
public:
	NsMonIp4Address (uint32_t ipv4);

	NsMonIp4Address (const std::string &ipv4str);

	NsMonIp4Address (const char *ipv4char);

	uint32_t ipv4 () const;
};

class NsMonitoringTypeBase {
public:
	virtual void writeTo (std::ostream *os) = 0;
};

template<typename T>
class NsMonitoringType : public NsMonitoringTypeBase {
public:

	NsMonitoringType (const T &t);

	virtual void writeTo (std::ostream *os) {
	}
};

class NsMonitoringArray : public std::vector<std::shared_ptr<NsMonitoringTypeBase> > {
public:

	template<typename T>
	void add (const T &val) {
		std::shared_ptr<NsMonitoringType<T> > tptr = std::make_shared<NsMonitoringType<T> > (val);
		std::shared_ptr<NsMonitoringTypeBase> bptr = std::static_pointer_cast<NsMonitoringTypeBase, NsMonitoringType<T> > (tptr);
		push_back (bptr);
	}

	~NsMonitoringArray () {
		while (size () > 0) {
			pop_back ();
		}
	}
};

template<>
class NsMonitoringType<char> : public NsMonitoringTypeBase {
private:
	char _v;
public:

	NsMonitoringType<char> (const char &v) {
		_v = v;
	}

	virtual void writeTo (std::ostream *os) {
		os->write ((char *) &_v, sizeof (_v));
	}
};

template<>
class NsMonitoringType<uint8_t> : public NsMonitoringTypeBase {
private:
	uint8_t _v;
public:

	NsMonitoringType<uint8_t> (const uint8_t &v) {
		_v = v;
	}

	virtual void writeTo (std::ostream *os) {
		os->write ((char *) &_v, sizeof (_v));
	}
};

template<>
class NsMonitoringType<int8_t> : public NsMonitoringTypeBase {
private:
	int8_t _v;
public:

	NsMonitoringType<int8_t> (const int8_t &v) {
		_v = v;
	}

	virtual void writeTo (std::ostream *os) {
		os->write ((char *) &_v, sizeof (_v));
	}
};

template<>
class NsMonitoringType<uint16_t> : public NsMonitoringTypeBase {
private:
	uint16_t _v;
public:

	NsMonitoringType<uint16_t> (const uint16_t &v) {
		_v = v;
	}

	virtual void writeTo (std::ostream *os) {
		uint16_t netv = ::htons (_v);
		os->write ((char *) &netv, sizeof (netv));
	}
};

template<>
class NsMonitoringType<int16_t> : public NsMonitoringTypeBase {
private:
	int16_t _v;
public:

	NsMonitoringType<int16_t> (const int16_t &v) {
		_v = v;
	}

	virtual void writeTo (std::ostream *os) {
		int16_t netv = ::htons (_v);
		os->write ((char *) &netv, sizeof (netv));
	}
};

template<>
class NsMonitoringType<uint32_t> : public NsMonitoringTypeBase {
private:
	uint32_t _v;
public:

	NsMonitoringType<uint32_t> (const uint32_t &v) {
		_v = v;
	}

	virtual void writeTo (std::ostream *os) {
		uint32_t netv = ::htonl (_v);
		os->write ((char *) &netv, sizeof (netv));
	}
};

template<>
class NsMonitoringType<int32_t> : public NsMonitoringTypeBase {
private:
	int32_t _v;
public:

	NsMonitoringType<int32_t> (const int32_t &v) {
		_v = v;
	}

	virtual void writeTo (std::ostream *os) {
		int32_t netv = ::htonl (_v);
		os->write ((char *) &netv, sizeof (netv));
	}
};

template<>
class NsMonitoringType<uint64_t> : public NsMonitoringTypeBase {
private:
	uint64_t _v;
public:

	NsMonitoringType<uint64_t> (const uint64_t &v) {
		_v = v;
	}

	virtual void writeTo (std::ostream *os) {
		uint64_t netv = htobe64 (_v);

		os->write ((char *) &netv, sizeof (netv));
	}
};

template<>
class NsMonitoringType<int64_t> : public NsMonitoringTypeBase {
private:
	int64_t _v;
public:

	NsMonitoringType<int64_t> (const int64_t &v) {
		_v = v;
	}

	virtual void writeTo (std::ostream *os) {
		int64_t netv = htobe64 (_v);

		os->write ((char *) &netv, sizeof (netv));
	}
};

template<>
class NsMonitoringType<std::string> : public NsMonitoringTypeBase {
private:
	std::shared_ptr<std::string> _s;
public:

	NsMonitoringType<std::string> (const std::string &s) {
		this->_s = std::make_shared<std::string> (s);
	}

	NsMonitoringType<std::string> (std::shared_ptr<std::string> s) {
		this->_s = s;
	}

	virtual void writeTo (std::ostream *os) {
		uint32_t len32 = _s->length ();
		if (len32 > UINT8_MAX) {
			len32 = UINT8_MAX;
		}

		uint8_t len8 = (uint8_t) len32;

		os->write ((char *) &len8, sizeof (len8));
		os->write (_s->c_str (), len8);
	}
};

template<>
class NsMonitoringType<NsMonIp4Address> : public NsMonitoringTypeBase {
private:
	uint32_t _v;
public:

	NsMonitoringType<NsMonIp4Address> (const NsMonIp4Address &v) {
		_v = v.ipv4 ();
	}

	virtual void writeTo (std::ostream *os) {
		uint32_t netv = ::htonl (_v);
		os->write ((char *) &netv, sizeof (netv));
	}
};

template<>
class NsMonitoringType<NsMonitoringArray> : public NsMonitoringTypeBase {
private:
	NsMonitoringArray _s;
public:

	NsMonitoringType<NsMonitoringArray> (const NsMonitoringArray &s) : _s (s) {
	}

	virtual void writeTo (std::ostream *os) {
		size_t len = _s.size ();
		if (len > UINT16_MAX) {
			len = UINT16_MAX;
		}

		uint16_t len16 = (uint16_t) len;
		uint16_t netlen16 = ::htons (len16);

		os->write ((char *) &netlen16, sizeof (netlen16));
		for (NsMonitoringArray::iterator it = _s.begin (); it != _s.end (); it++) {
			(*it)->writeTo (os);
		}
	}
};

class NsMonitoring {
public:

	static bool enabled ();

	template <typename T1>
	static void send (const T1 &arg1) {
		if (!enabled ()) {
			return;
		}

		NsMonitoringType<T1> ma1 (arg1);

		std::stringstream * os = sendMonStart ();
		ma1.writeTo (os);
		doSendMon (os);
	}

	template <typename T1, typename T2>
	static void send (const T1 &arg1, const T2 &arg2) {
		if (!enabled ()) {
			return;
		}

		NsMonitoringType<T1> ma1 (arg1);
		NsMonitoringType<T2> ma2 (arg2);

		std::stringstream *os = sendMonStart ();
		ma1.writeTo (os);
		ma2.writeTo (os);
		doSendMon (os);
	}

	template <typename T1, typename T2, typename T3>
	static void send (const T1 &arg1, const T2 &arg2, const T3 &arg3) {
		if (!enabled ()) {
			return;
		}

		NsMonitoringType<T1> ma1 (arg1);
		NsMonitoringType<T2> ma2 (arg2);
		NsMonitoringType<T3> ma3 (arg3);

		std::stringstream *os = sendMonStart ();
		ma1.writeTo (os);
		ma2.writeTo (os);
		ma3.writeTo (os);
		doSendMon (os);
	}

	template <typename T1, typename T2, typename T3, typename T4>
	static void send (const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4) {
		if (!enabled ()) {
			return;
		}

		NsMonitoringType<T1> ma1 (arg1);
		NsMonitoringType<T2> ma2 (arg2);
		NsMonitoringType<T3> ma3 (arg3);
		NsMonitoringType<T4> ma4 (arg4);

		std::stringstream *os = sendMonStart ();
		ma1.writeTo (os);
		ma2.writeTo (os);
		ma3.writeTo (os);
		ma4.writeTo (os);
		doSendMon (os);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5>
	static void send (const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5) {
		if (!enabled ()) {
			return;
		}

		NsMonitoringType<T1> ma1 (arg1);
		NsMonitoringType<T2> ma2 (arg2);
		NsMonitoringType<T3> ma3 (arg3);
		NsMonitoringType<T4> ma4 (arg4);
		NsMonitoringType<T5> ma5 (arg5);

		std::stringstream *os = sendMonStart ();
		ma1.writeTo (os);
		ma2.writeTo (os);
		ma3.writeTo (os);
		ma4.writeTo (os);
		ma5.writeTo (os);
		doSendMon (os);
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	static void send (const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4, const T5 &arg5, const T6 &arg6) {
		if (!enabled ()) {
			return;
		}

		NsMonitoringType<T1> ma1 (arg1);
		NsMonitoringType<T2> ma2 (arg2);
		NsMonitoringType<T3> ma3 (arg3);
		NsMonitoringType<T4> ma4 (arg4);
		NsMonitoringType<T5> ma5 (arg5);
		NsMonitoringType<T6> ma6 (arg6);

		std::stringstream *os = sendMonStart ();
		ma1.writeTo (os);
		ma2.writeTo (os);
		ma3.writeTo (os);
		ma4.writeTo (os);
		ma5.writeTo (os);
		ma6.writeTo (os);
		doSendMon (os);
	}

	static void monitorStartup ();
	static void monitorShutdown ();
	static void monitorSendRpcRequest (std::shared_ptr<std::string> remoteServiceName, std::shared_ptr<std::string> remoteMethod, const uint64_t requestId);
	static void monitorSendRpcRequestError (std::shared_ptr<std::string> remoteServiceName, std::shared_ptr<std::string> remoteMethod, const uint64_t requestId, const char *errorDescription);
	static void monitorReceiveRpcResponse (const uint64_t requestId);
	static void monitorReceiveRpcResponseError (const uint64_t requestId, const char *errorDescription);
	static void monitorReceiveRpcRequest (const uint64_t requestId, std::shared_ptr<std::string> method);
	static void monitorReceiveRpcRequestError (const char *errorDescription);
	static void monitorSendRpcResponse (const uint64_t requestId);
	static void monitorSendRpcResponseError (const char *errorDescription);

private:
	static std::stringstream *sendMonStart ();
	static void doSendMon (std::stringstream *os);
};

}

#endif /* NSMONITORING_H */

