#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"
#include<bits/stdc++.h>

// Default Network Topology
//
//       10.1.1.0
// n0 -------------- n1   n2   n3   n4
//    point-to-point  |    |    |    |
//                    ================
//                      LAN 10.1.2.0


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SecondScriptExample");

int main (int argc, char *argv[])
{
  bool verbose = true;
  uint32_t nCsma = 3;

  CommandLine cmd;
  cmd.AddValue ("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
  cmd.AddValue ("verbose", "Tell echo applications to log if true", verbose);

  cmd.Parse (argc,argv);

  if (verbose)
    {
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

  nCsma = nCsma == 0 ? 1 : nCsma;

  NodeContainer p2pNodes;
  p2pNodes.Create (2);

  NodeContainer csmaNodes;
  csmaNodes.Add (p2pNodes.Get (1));
  csmaNodes.Create (nCsma);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("6Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install (p2pNodes);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("101Mbps"));
  csma.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer csmaDevices;
  csmaDevices = csma.Install (csmaNodes);

  InternetStackHelper stack;
  stack.Install (p2pNodes.Get (0));
  stack.Install (csmaNodes);

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces;
  p2pInterfaces = address.Assign (p2pDevices);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces;
  csmaInterfaces = address.Assign (csmaDevices);

  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (csmaNodes.Get (nCsma));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (15.0));

  UdpEchoClientHelper echoClient (csmaInterfaces.GetAddress (nCsma), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (3));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (0.2)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (2048));

  ApplicationContainer clientApps = echoClient.Install (p2pNodes.Get (0));
  clientApps.Start (Seconds (1.0));
  clientApps.Stop (Seconds (15.0));


 ApplicationContainer clientApps1 = echoClient.Install (csmaNodes.Get (0));
  clientApps1.Start (Seconds (1.5));
  clientApps1.Stop (Seconds (15.0));

  ApplicationContainer clientApps2 = echoClient.Install (csmaNodes.Get (1));
  clientApps2.Start (Seconds (2.0));
  clientApps2.Stop (Seconds (15.0));

  ApplicationContainer clientApps3 = echoClient.Install (csmaNodes.Get (2));
  clientApps3.Start (Seconds (3.0));
  clientApps3.Stop (Seconds (15.0));


  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  pointToPoint.EnablePcapAll ("p2p");
  csma.EnablePcap ("csma1", csmaDevices.Get (1), true);
  csma.EnablePcap ("csma3", csmaDevices.Get (3), true);

 AnimationInterface anim("second.xml");
 anim.SetConstantPosition(p2pNodes.Get(0),10.0,10.0);
anim.SetConstantPosition(p2pNodes.Get(1),20.0,20.0);
anim.SetConstantPosition(csmaNodes.Get(1),30.0,30.0);
anim.SetConstantPosition(csmaNodes.Get(2),40.0,40.0);
anim.SetConstantPosition(csmaNodes.Get(3),50.0,50.0);


  AsciiTraceHelper ascii;
  pointToPoint.EnableAsciiAll(ascii.CreateFileStream("point2point.tr"));
  csma.EnableAsciiAll(ascii.CreateFileStream("csma2.tr"));

 // Simulator::Run ();

FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();


//double t1=0;
//ns3::int64x64_t t2=0;
//
// Now, do the actual simulation.
//
  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds(15.0));
  Simulator::Run ();

  monitor->CheckForLostPackets ();

  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
         // std::cout << "Flow ";
 Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
      //if (((t.sourceAddress=="10.1.1.1" || t.sourceAddress=="10.1.2.3" || t.sourceAddress=="10.1.2.2" || t.sourceAddress=="10.1.2.1") && t.destinationAddress == "10.1.2.4"))
      //{
          std::cout<<"\n\n";
          std::cout << "Data transmission " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
         // std::cout << "  Transmittedx Bytes:   " << i->second.txBytes << "\n";
         // std::cout << "  Recievedx Bytes:   " << i->second.rxBytes << "\n";
          // std::cout << "  Last Packet Recieved:   " << std::setprecision(5)<<i->second.timeLastRxPacket.GetSeconds()  << "\n";
        //   std::cout << "  First Packet Sent:   " <<std::setprecision(5)<<i->second.timeFirstTxPacket.GetSeconds()  << "\n";

      //std::cout<<"Duration : "<<i->second.timeLastRxPacket.GetSeconds()-i->second.timeFirstTxPacket.GetSeconds()<<std::endl;


       std::cout << "  Throughput: " << std::setprecision(5)<<i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps\n";
                //std::cout<<" second.delaysum: "<<i->second.delaySum<<"\n";
                 //std::cout<<"second.rxbytes"<<i->second.rxBytes<<"\n";
          std::cout<<" Average Delay: "<<i->second.delaySum/(i->second.rxBytes/1024);

      //}
          std::cout<<"\n";
     }



  monitor->SerializeToXmlFile("lab-1.flowmon", true, true);
  Simulator::Destroy ();
  return 0;
}
