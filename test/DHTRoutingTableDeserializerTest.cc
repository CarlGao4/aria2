#include "DHTRoutingTableDeserializer.h"
#include "DHTRoutingTableSerializer.h"
#include "Exception.h"
#include "Util.h"
#include "DHTNode.h"
#include "a2functional.h"
#include "DHTConstants.h"
#include "PeerMessageUtil.h"
#include <arpa/inet.h>
#include <cstring>
#include <sstream>
#include <cppunit/extensions/HelperMacros.h>

class DHTRoutingTableDeserializerTest:public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE(DHTRoutingTableDeserializerTest);
  CPPUNIT_TEST(testDeserialize);
  CPPUNIT_TEST_SUITE_END();
public:
  void setUp() {}

  void tearDown() {}

  void testDeserialize();
};


CPPUNIT_TEST_SUITE_REGISTRATION(DHTRoutingTableDeserializerTest);

void DHTRoutingTableDeserializerTest::testDeserialize()
{
  DHTNodeHandle localNode = new DHTNode();

  DHTNodeHandle nodesSrc[] = { 0, 0, 0 };
  for(size_t i = 0; i < arrayLength(nodesSrc); ++i) {
    nodesSrc[i] = new DHTNode();
    nodesSrc[i]->setIPAddress("192.168.0."+Util::uitos(i+1));
    nodesSrc[i]->setPort(6881+i);
  }
  nodesSrc[1]->setIPAddress("non-numerical-name");
  DHTNodes nodes(&nodesSrc[0], &nodesSrc[arrayLength(nodesSrc)]);
  
  DHTRoutingTableSerializer s;
  s.setLocalNode(localNode);
  s.setNodes(nodes);

  stringstream ss;
  s.serialize(ss);

  DHTRoutingTableDeserializer d;
  d.deserialize(ss);

  CPPUNIT_ASSERT(memcmp(localNode->getID(), d.getLocalNode()->getID(),
			DHT_ID_LENGTH) == 0);

  cout << d.getSerializedTime().getTime() << endl;

  CPPUNIT_ASSERT_EQUAL((size_t)2, d.getNodes().size());
  const DHTNodes& dsnodes = d.getNodes();
  CPPUNIT_ASSERT_EQUAL(string("192.168.0.1"), dsnodes[0]->getIPAddress());
  CPPUNIT_ASSERT_EQUAL((uint16_t)6881, dsnodes[0]->getPort());
  CPPUNIT_ASSERT(memcmp(nodes[0]->getID(), dsnodes[0]->getID(), DHT_ID_LENGTH) == 0);
  CPPUNIT_ASSERT_EQUAL(string("192.168.0.3"), dsnodes[1]->getIPAddress());
  CPPUNIT_ASSERT_EQUAL((uint16_t)6883, dsnodes[1]->getPort());
  CPPUNIT_ASSERT(memcmp(nodes[2]->getID(), dsnodes[1]->getID(), DHT_ID_LENGTH) == 0);
}
