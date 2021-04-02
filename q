BYTEORDER(3)                                                        Linux Programmer's Manual                                                       BYTEORDER(3)

NNAAMMEE
       htonl, htons, ntohl, ntohs - convert values between host and network byte order

SSYYNNOOPPSSIISS
       ##iinncclluuddee <<aarrppaa//iinneett..hh>>

       uuiinntt3322__tt hhttoonnll((uuiinntt3322__tt _h_o_s_t_l_o_n_g));;

       uuiinntt1166__tt hhttoonnss((uuiinntt1166__tt _h_o_s_t_s_h_o_r_t));;

       uuiinntt3322__tt nnttoohhll((uuiinntt3322__tt _n_e_t_l_o_n_g));;

       uuiinntt1166__tt nnttoohhss((uuiinntt1166__tt _n_e_t_s_h_o_r_t));;

DDEESSCCRRIIPPTTIIOONN
       The hhttoonnll() function converts the unsigned integer _h_o_s_t_l_o_n_g from host byte order to network byte order.

       The hhttoonnss() function converts the unsigned short integer _h_o_s_t_s_h_o_r_t from host byte order to network byte order.

       The nnttoohhll() function converts the unsigned integer _n_e_t_l_o_n_g from network byte order to host byte order.

       The nnttoohhss() function converts the unsigned short integer _n_e_t_s_h_o_r_t from network byte order to host byte order.

       On the i386 the host byte order is Least Significant Byte first, whereas the network byte order, as used on the Internet, is Most Significant Byte first.

AATTTTRRIIBBUUTTEESS
       For an explanation of the terms used in this section, see aattttrriibbuutteess(7).

       ┌───────────────────────────────────┬───────────────┬─────────┐
       │IInntteerrffaaccee                          │ AAttttrriibbuuttee     │ VVaalluuee   │
       ├───────────────────────────────────┼───────────────┼─────────┤
       │hhttoonnll(), hhttoonnss(), nnttoohhll(), nnttoohhss() │ Thread safety │ MT-Safe │
       └───────────────────────────────────┴───────────────┴─────────┘
CCOONNFFOORRMMIINNGG TTOO
       POSIX.1-2001, POSIX.1-2008.

       Some systems require the inclusion of _<_n_e_t_i_n_e_t_/_i_n_._h_> instead of _<_a_r_p_a_/_i_n_e_t_._h_>.

SSEEEE AALLSSOO
       bbsswwaapp(3), eennddiiaann(3), ggeetthhoossttbbyynnaammee(3), ggeettsseerrvveenntt(3)

CCOOLLOOPPHHOONN
       This page is part of release 5.05 of the Linux _m_a_n_-_p_a_g_e_s project.  A description of the project, information about reporting bugs, and the latest version
       of this page, can be found at https://www.kernel.org/doc/man-pages/.

GNU                                                                        2017-09-15                                                               BYTEORDER(3)
