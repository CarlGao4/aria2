#include "DHTFindNodeReplyMessage.h"
#include "DHTNode.h"
#include "DHTUtil.h"
#include "BencodeVisitor.h"
#include "Dictionary.h"
#include "Data.h"
#include "Exception.h"
#include "Util.h"
#include "DHTBucket.h"
#include "PeerMessageUtil.h"
#include <cppunit/extensions/HelperMacros.h>

class DHTFindNodeReplyMessageTest:public CppUnit::TestFixture {

  CPPUNIT_TEST_SUITE(DHTFindNodeReplyMessageTest);
  CPPUNIT_TEST(testGetBencodedMessage);
  CPPUNIT_TEST_SUITE_END();
public:
  void setUp() {}

  void tearDown() {}

  void testGetBencodedMessage();
};


CPPUNIT_TEST_SUITE_REGISTRATION(DHTFindNodeReplyMessageTest);

void DHTFindNodeReplyMessageTest::testGetBencodedMessage()
{
  DHTNodeHandle localNode = new DHTNode();
  DHTNodeHandle remoteNode = new DHTNode();

  char tid[DHT_TRANSACTION_ID_LENGTH];
  DHTUtil::generateRandomData(tid, DHT_TRANSACTION_ID_LENGTH);
  string transactionID(&tid[0], &tid[DHT_TRANSACTION_ID_LENGTH]);

  DHTFindNodeReplyMessage msg(localNode, remoteNode, transactionID);

  string compactNodeInfo;
  DHTNodeHandle nodes[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
  for(size_t i = 0; i < DHTBucket::K; ++i) {
    nodes[i] = new DHTNode();
    nodes[i]->setIPAddress("192.168.0."+Util::uitos(i+1));
    nodes[i]->setPort(6881+i);

    char buf[6];
    CPPUNIT_ASSERT(PeerMessageUtil::createcompact(buf, nodes[i]->getIPAddress(), nodes[i]->getPort()));
    compactNodeInfo +=
      string(&nodes[i]->getID()[0], &nodes[i]->getID()[DHT_ID_LENGTH])+
      string(&buf[0], &buf[sizeof(buf)]);
  }
  msg.setClosestKNodes(DHTNodes(&nodes[0], &nodes[DHTBucket::K]));

  string msgbody = msg.getBencodedMessage();

  SharedHandle<Dictionary> cm = new Dictionary();
  cm->put("t", new Data(transactionID));
  cm->put("y", new Data("r"));
  Dictionary* r = new Dictionary();
  cm->put("r", r);
  r->put("id", new Data(reinterpret_cast<const char*>(localNode->getID()), DHT_ID_LENGTH));
  r->put("nodes", new Data(compactNodeInfo));

  BencodeVisitor v;
  cm->accept(&v);

  CPPUNIT_ASSERT_EQUAL(v.getBencodedData(), msgbody);
}
