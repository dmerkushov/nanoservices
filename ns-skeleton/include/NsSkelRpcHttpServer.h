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
 * File:   NsSkelRpcHttpServer.h
 * Author: dmerkushov
 *
 * Created on June 24, 2019, 5:07 PM
 */

#ifndef NSSKELRPCHTTPSERVER_H
#define NSSKELRPCHTTPSERVER_H

#include "NsSkelRpcServer.h"

namespace nanoservices {

class NsSkelRpcHttpServer : public NsSkelRpcServer {
public:
	NsSkelRpcHttpServer ();
	virtual ~NsSkelRpcHttpServer ();

protected:
	virtual void processIncomingConnection (int dataSocketFd);

private:
	NsSkelRpcHttpServer (const NsSkelRpcHttpServer& orig) = delete;
};

}

#endif /* NSSKELRPCHTTPSERVER_H */

