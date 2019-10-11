#include <iostream>
#include <string>
#include "smpHttp.hpp"
#include "Mysql_con.hpp"
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using namespace std;
using namespace ::mysqlx;
Mysql_con mq("47.103.210.8", "root", "lab", "root");

void handleJson(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{
  auto sqlres = mq.sql("Select content FROM labimformation where type = 'labIntroduction'").execute();
  auto i = sqlres.fetchOne();
  cout << i.get(0) << endl;
  res->addHeader("Content-Type","application/json;charset=UTF-8");
  json j;
  auto p = json::parse(R"([
    "计算机网络",
    "移动通信",
    "移动互联网开发"])");
  j["labIntroduction"] = "中国科学技术大学信息网络实验室（Lab. of Information Network，InfoNet）为无线网络通信安徽省重点实验室的成员单位之一，由李津生教授创建于1988年，原名ISDN实验室。自成立以来，实验室一直致力于网络基础理论研究和关键技术开发，先后承担了国家自然科学基金、科技部重大专项、863课题、中国下一代互联网CNGI项目、国家攻关课题等数十项项国家、军队和省部级重大课题。实验室是国内较早从事IPv6技术研究和应用的科研单位之一。目前，在对新一代网络技术及其应用的研究中，实验室主要课题方向包括：未来网络系统结构、智能电网、特定网络环境下的多媒体优化传输、无线网络技术、网络移动性管理、网络安全等等。在研项目包括：国家自然科学基金项目2项、科技部重大专项3项、国防科工类项目3项和一系列横向合作课题。实验室还承担学校本科生和研究生的教学工作， 教学内容注重与网络技术发展同步， 并及时将科研成果反映到课堂教学和实验中。 所开设课程包括信息学院院定必修课—— 计算机网络、 信息安全专业必修课程—— 网络安全协议和通信与信息系统专业研究生必修课——信息网络与协议。实验室现有教授1名，副教授2名，在读博士研究生12人、硕士研究生32人";
  j["labDirection"] = p;
  std::string jso = j.dump();
  res->addMessage(jso);
}

int main(int argc, const char* argv[]) {
  smpHttp::HttpServer server;
  server.add_route("/json", handleJson);
  server.add_static_path("/resources"); //add static route
  server.run();
}
