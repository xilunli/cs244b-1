#include "network.h"

void NetworkInstance::initSocket()
{
    sockaddr_in		nullAddr;
    sockaddr_in		*thisHost;
    char			buf[128];
    int				reuse;
    u_char          ttl;
    struct ip_mreq  mreq;

    gethostname(buf, sizeof(buf));
    if ((thisHost = resolveHost(buf)) == (sockaddr_in *) NULL) {
        throw FSException("who am I?");
    }
    bcopy((caddr_t) thisHost, (caddr_t) &myAddr, sizeof(sockaddr_in));

    if((mySocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        throw FSException("can't get socket");
    }

    /* SO_REUSEADDR allows more than one binding to the same
       socket - you cannot have more than one player on one
       machine without this */
    reuse = 1;
    if (setsockopt(mySocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        FSException("setsockopt failed (SO_REUSEADDR)");
    }

    nullAddr.sin_family = AF_INET;
    nullAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    nullAddr.sin_port = htons(port);
    if (bind(mySocket, (struct sockaddr *)&nullAddr, sizeof(nullAddr)) < 0) {
        FSException("netInit binding failed.");
    }

    /* Multicast TTL: DO NOT use a value > 32. Use 1 when testing. */

    ttl = 1;
    if (setsockopt(mySocket, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
        FSException("setsockopt failed (IP_MULTICAST_TTL)");
    }

    /* join the multicast group */
    mreq.imr_multiaddr.s_addr = htonl(group);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(mySocket, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                   (char *)&mreq, sizeof(mreq)) < 0) {
        FSException("setsockopt failed (IP_ADD_MEMBERSHIP)");
    }

}

sockaddr_in *NetworkInstance::resolveHost(register char *name)
{
    register struct hostent *fhost;
    struct in_addr fadd;
    static sockaddr_in sa;

    if ((fhost = gethostbyname(name)) != NULL) {
        sa.sin_family = fhost->h_addrtype;
        sa.sin_port = 0;
        bcopy(fhost->h_addr, &sa.sin_addr, fhost->h_length);
    } else {
        if (inet_aton(name, &fadd) != 0) {
            sa.sin_family = AF_INET;	/* grot */
            sa.sin_port = 0;
            sa.sin_addr.s_addr = fadd.s_addr;
        } else
            return(NULL);
    }
    return(&sa);
}

