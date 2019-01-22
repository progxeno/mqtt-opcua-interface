#!/bin/bash

clear
SSID="${1:-MasterarbeitPi}"
PASSPHRASE="${2:-MasterarbeitPi@iot}"
IP_RANGE="${3:-10.0.0}"


echo "Setting up your WiFi-Accesspoint on your pi with:"
echo " SSID: $SSID"
echo " PASSPHRASE: $PASSPHRASE"
echo " IP-Address: $IP_RANGE.1"
echo " IP-Range: $IP_RANGE.0"

echo "===Installation menu==="
PS3='Choose option:'
options=("option1 install access point." "option2 uninstall access point." "option 3 exit.")
select pez in "${options[@]}"
do
    case $pez in
    "option1 install access point." )
      sudo apt-get -y update 
      sudo apt-get -y upgrade 
      sudo apt-get -y install hostapd #install hostapd
      sudo apt-get -y install dnsmasq #install dnsmasq
      cat /dev/null > /etc/dhcpcd.conf
      echo " # A sample configuration for dhcpcd.
      # See dhcpcd.conf(5) for details.
      # Allow users of this group to interact with dhcpcd via the control socket.
      #controlgroup wheel
      # Inform the DHCP server of our hostname for DDNS.
      hostname
      # Use the hardware address of the interface for the Client ID.
      clientid
      # or
      # Use the same DUID + IAID as set in DHCPv6 for DHCPv4 ClientID as per RFC4361.
      #duid
      # Persist interface configuration when dhcpcd exits.
      persistent
      # Rapid commit support.
      # Safe to enable by default because it requires the equivalent option set
      # on the server to actually work.
      option rapid_commit
      # A list of options to request from the DHCP server.
      option domain_name_servers, domain_name, domain_search, host_name
      option classless_static_routes
      # Most distributions have NTP support.
      option ntp_servers
      # Respect the network MTU.
      # Some interface drivers reset when changing the MTU so disabled by default.
      #option interface_mtu
      # A ServerID is required by RFC2131.
      require dhcp_server_identifier
      # Generate Stable Private IPv6 Addresses instead of hardware based ones
      slaac private
      # A hook script is provided to lookup the hostname if not set by the DHCP
      # server, but it should not be run by default.
      interface eth0
      static ip_address=$IP_RANGE.2/24
      static routers=$IP_RANGE.1
      static domain_name_servers=$IP_RANGE.1

      interface wlan0
      static ip_address=$IP_RANGE.1/24
      static routers=$IP_RANGE.1
      static domain_name_servers=$IP_RANGE.1">>/etc/dhcpcd.conf
      cat /dev/null > /etc/network/interfaces
      echo "# interfaces(5) file used by ifup(8) and ifdown(8)
      # Please note that this file is written to be used with dhcpcd
      # For static IP, consult /etc/dhcpcd.conf and 'man dhcpcd.conf'
      # Include files from /etc/network/interfaces.d:
      source-directory /etc/network/interfaces.d

      allow-hotplug eth0
      iface eth0 inet manual

      allow-hotplug wlan0
      iface wlan0 inet manual

      wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf">>/etc/network/interfaces
      sudo service dhcpcd restart
      sudo ifdown wlan0
      sudo ifup wlan0
      cat /etc/hostapd/hostapd.conf
      echo "interface=wlan0
ssid=$SSID
hw_mode=g
channel=3
ieee80211n=1
wmm_enabled=0
ht_capab=[HT40][SHORT-GI-20][DSSS_CCK-40]
auth_algs=1
wpa=2
wpa_key_mgmt=WPA-PSK
wpa_passphrase=$PASSPHRASE
rsn_pairwise=CCMP
wpa_pairwise=TKIP">>/etc/hostapd/hostapd.conf
      cat /dev/null > /etc/default/hostapd
      echo '# Defaults for hostapd initscript
      #
      # See /usr/share/doc/hostapd/README.Debian for information about alternative
      # methods of managing hostapd.
      #
      # Uncomment and set DAEMON_CONF to the absolute path of a hostapd configuration
      # file and hostapd will be started during system boot. An example configuration
      # file can be found at /usr/share/doc/hostapd/examples/hostapd.conf.gz
      #
      DAEMON_CONF="/etc/hostapd/hostapd.conf"
      # Additional daemon options to be appended to hostapd command:-
      # 	-d   show more debug messages (-dd for even more)
      # 	-K   include key data in debug messages
      # 	-t   include timestamps in some debug messages
      #
      # Note that -B (daemon mode) and -P (pidfile) options are automatically
      # configured by the init.d script and must not be added to DAEMON_OPTS.
      #
      #DAEMON_OPTS=""" '>>/etc/default/hostapd
      sudo mv /etc/dnsmasq.conf /etc/dnsmasq.conf.orig
      touch /etc/dnsmasq.conf
      echo "interface=wlan0
      dhcp-range=$IP_RANGE.5,$IP_RANGE.200,255.255.255.0,24h ">>/etc/dnsmasq.conf
      cat /dev/null > /etc/sysctl.conf
      echo "#
      # /etc/sysctl.conf - Configuration file for setting system variables
      # See /etc/sysctl.d/ for additional system variables.
      # See sysctl.conf (5) for information.
      #
      #kernel.domainname = example.com
      # Uncomment the following to stop low-level messages on console
      #kernel.printk = 3 4 1 3
      ##############################################################3
      # Functions previously found in netbase
      #
      # Uncomment the next two lines to enable Spoof protection (reverse-path filter)
      # Turn on Source Address Verification in all interfaces to
      # prevent some spoofing attacks
      #net.ipv4.conf.default.rp_filter=1
      #net.ipv4.conf.all.rp_filter=1
      # Uncomment the next line to enable TCP/IP SYN cookies
      # See http://lwn.net/Articles/277146/
      # Note: This may impact IPv6 TCP sessions too
      #net.ipv4.tcp_syncookies=1
      # Uncomment the next line to enable packet forwarding for IPv4
      net.ipv4.ip_forward=1
      # Uncomment the next line to enable packet forwarding for IPv6
      #  Enabling this option disables Stateless Address Autoconfiguration
      #  based on Router Advertisements for this host
      #net.ipv6.conf.all.forwarding=1
      ###################################################################
      # Additional settings - these settings can improve the network
      # security of the host and prevent against some network attacks
      # including spoofing attacks and man in the middle attacks through
      # redirection. Some network environments, however, require that these
      # settings are disabled so review and enable them as needed.
      #
      # Do not accept ICMP redirects (prevent MITM attacks)
      #net.ipv4.conf.all.accept_redirects = 0
      #net.ipv6.conf.all.accept_redirects = 0
      # _or_
      # Accept ICMP redirects only for gateways listed in our default
      # gateway list (enabled by default)
      # net.ipv4.conf.all.secure_redirects = 1
      #
      # Do not send ICMP redirects (we are not a router)
      #net.ipv4.conf.all.send_redirects = 0
      #
      # Do not accept IP source route packets (we are not a router)
      #net.ipv4.conf.all.accept_source_route = 0
      #net.ipv6.conf.all.accept_source_route = 0
      #
      # Log Martian Packets
      #net.ipv4.conf.all.log_martians = 1
      #">>/etc/sysctl.conf
      sudo sh -c "echo 1 > /proc/sys/net/ipv4/ip_forward"
      sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
      sudo iptables -A FORWARD -i eth0 -o wlan0 -m state --state RELATED,ESTABLISHED -j ACCEPT
      sudo iptables -A FORWARD -i wlan0 -o eth0 -j ACCEPT
      sudo sh -c "iptables-save > /etc/iptables.ipv4.nat"
      cat /dev/null > /etc/rc.local
      echo "#!/bin/sh -e
      #
      # rc.local
      #
      # This script is executed at the end of each multiuser runlevel.
      # Make sure that the script will "exit 0" on success or any other
      # value on error.
      #
      # In order to enable or disable this script just change the execution
      # bits.
      #
      # By default this script does nothing.
      # Print the IP address
      _IP=$(hostname -I) || true
      if [ "$_IP" ]; then
        printf "My IP address is %s\n" "$_IP"
      fi
     dmesg --console-off
      iptables-restore < /etc/iptables.ipv4.nat
      exit 0">>/etc/rc.local
      sudo service hostapd start
      sudo service dnsmasq start
	  sudo cp /etc/hosts /etc/hosts.old
	  echo "$IP_RANGE.1	raspberrypi">>/etc/hosts
	  sudo cp /etc/wpa_supplicant/wpa_supplicant.conf /etc/wpa_supplicant/wpa_supplicant.conf.old
	  cat /dev/null > /etc/wpa_supplicant/wpa_supplicant.conf
      echo "ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
	  update_config=1
	  country=DE">>/etc/wpa_supplicant/wpa_supplicant.conf
      sudo reboot
      ;;
    "option2 uninstall access point." )
    echo "elegiste la option 2"
    #!/bin/bash
    #uninstall access point
    clear
    sudo apt-get remove hostapd
    sudo apt-get remove dnsmasq
    cat /dev/null > /etc/dhcpcd.conf
    echo"# A sample configuration for dhcpcd.
    # See dhcpcd.conf(5) for details.
    # Allow users of this group to interact with dhcpcd via the control socket.
    #controlgroup wheel
    # Inform the DHCP server of our hostname for DDNS.
    hostname
    # Use the hardware address of the interface for the Client ID.
    clientid
    # or
    # Use the same DUID + IAID as set in DHCPv6 for DHCPv4 ClientID as per RFC4361.
    #duid
    # Persist interface configuration when dhcpcd exits.
    persistent
    # Rapid commit support.
    # Safe to enable by default because it requires the equivalent option set
    # on the server to actually work.
    option rapid_commit
    # A list of options to request from the DHCP server.
    option domain_name_servers, domain_name, domain_search, host_name
    option classless_static_routes
    # Most distributions have NTP support.
    option ntp_servers
    # Respect the network MTU.
    # Some interface drivers reset when changing the MTU so disabled by default.
    #option interface_mtu
    # A ServerID is required by RFC2131.
    require dhcp_server_identifier
    # Generate Stable Private IPv6 Addresses instead of hardware based ones
    slaac private
    # A hook script is provided to lookup the hostname if not set by the DHCP
    # server, but it should not be run by default.
    nohook lookup-hostname">>/etc/dhcpcd.conf
    cat /dev/null > /etc/network/interfaces
    echo "# interfaces(5) file used by ifup(8) and ifdown(8)
    # Please note that this file is written to be used with dhcpcd
    # For static IP, consult /etc/dhcpcd.conf and 'man dhcpcd.conf'
    # Include files from /etc/network/interfaces.d:
    source-directory /etc/network/interfaces.d
    auto lo
    iface lo inet loopback
    iface eth0 inet manual
    allow-hotplug wlan0
    iface wlan0 inet manual
        wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf
    allow-hotplug wlan1
    iface wlan1 inet manual
        wpa-conf /etc/wpa_supplicant/wpa_supplicant.conf">>/etc/network/interfaces
    sudo service dhcpcd restart
    sudo ifdown wlan0
    sudo ifup wlan0
    cat /dev/null > /etc/hostapd/hostapd.conf
    cat /dev/null > /etc/default/hostapd
    echo '# Defaults for hostapd initscript
    #
    # See /usr/share/doc/hostapd/README.Debian for information about alternative
    # methods of managing hostapd.
    #
    # Uncomment and set DAEMON_CONF to the absolute path of a hostapd configuration
    # file and hostapd will be started during system boot. An example configuration
    # file can be found at /usr/share/doc/hostapd/examples/hostapd.conf.gz
    #
    #DAEMON_CONF=""
    # Additional daemon options to be appended to hostapd command:-
    # 	-d   show more debug messages (-dd for even more)
    # 	-K   include key data in debug messages
    # 	-t   include timestamps in some debug messages
    #
    # Note that -B (daemon mode) and -P (pidfile) options are automatically
    # configured by the init.d script and must not be added to DAEMON_OPTS.
    #
    #DAEMON_OPTS=""" '>>/etc/default/hostapd
    sudo rm /etc/dnsmasq.conf
    sudo mv /etc/dnsmasq.conf.orig /etc/dnsmasq.conf
    cat /dev/null > /etc/sysctl.conf
    echo "#
    # /etc/sysctl.conf - Configuration file for setting system variables
    # See /etc/sysctl.d/ for additional system variables.
    # See sysctl.conf (5) for information.
    #
    #kernel.domainname = example.com
    # Uncomment the following to stop low-level messages on console
    #kernel.printk = 3 4 1 3
    ##############################################################3
    # Functions previously found in netbase
    #
    # Uncomment the next two lines to enable Spoof protection (reverse-path filter)
    # Turn on Source Address Verification in all interfaces to
    # prevent some spoofing attacks
    #net.ipv4.conf.default.rp_filter=1
    #net.ipv4.conf.all.rp_filter=1
    # Uncomment the next line to enable TCP/IP SYN cookies
    # See http://lwn.net/Articles/277146/
    # Note: This may impact IPv6 TCP sessions too
    #net.ipv4.tcp_syncookies=1
    # Uncomment the next line to enable packet forwarding for IPv4
    #net.ipv4.ip_forward=1
    # Uncomment the next line to enable packet forwarding for IPv6
    #  Enabling this option disables Stateless Address Autoconfiguration
    #  based on Router Advertisements for this host
    #net.ipv6.conf.all.forwarding=1
    ###################################################################
    # Additional settings - these settings can improve the network
    # security of the host and prevent against some network attacks
    # including spoofing attacks and man in the middle attacks through
    # redirection. Some network environments, however, require that these
    # settings are disabled so review and enable them as needed.
    #
    # Do not accept ICMP redirects (prevent MITM attacks)
    #net.ipv4.conf.all.accept_redirects = 0
    #net.ipv6.conf.all.accept_redirects = 0
    # _or_
    # Accept ICMP redirects only for gateways listed in our default
    # gateway list (enabled by default)
    # net.ipv4.conf.all.secure_redirects = 1
    #
    # Do not send ICMP redirects (we are not a router)
    #net.ipv4.conf.all.send_redirects = 0
    #
    # Do not accept IP source route packets (we are not a router)
    #net.ipv4.conf.all.accept_source_route = 0
    #net.ipv6.conf.all.accept_source_route = 0
    #
    # Log Martian Packets
    #net.ipv4.conf.all.log_martians = 1
    #">>/etc/sysctl.conf
    sudo sh -c "echo 1 > /proc/sys/net/ipv4/ip_forward"
    sudo iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE
    sudo iptables -A FORWARD -i eth0 -o wlan0 -m state --state RELATED,ESTABLISHED -j ACCEPT
    sudo iptables -A FORWARD -i wlan0 -o eth0 -j ACCEPT
    sudo sh -c "iptables-save > /etc/iptables.ipv4.nat"
    cat /dev/null > /etc/rc.local
    echo "#!/bin/sh -e
    #
    # rc.local
    #
    # This script is executed at the end of each multiuser runlevel.
    # Make sure that the script will "exit 0" on success or any other
    # value on error.
    #
    # In order to enable or disable this script just change the execution
    # bits.
    #
    # By default this script does nothing.
    # Print the IP address
    _IP=$(hostname -I) || true
    if [ "$_IP" ]; then
      printf "My IP address is %s\n" "$_IP"
    fi
    exit 0">>/etc/rc.local
	
	sudo cp /etc/hosts.old /etc/hosts
	sudo cp /etc/wpa_supplicant/wpa_supplicant.conf.old /etc/wpa_supplicant/wpa_supplicant.conf
	  
    sudo reboot
    ;;
    "option 3 exit." )
    echo "exit"
    break
    ;;
    *) echo option invalida;;
  esac
done
