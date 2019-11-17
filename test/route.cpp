#include "route.hpp"

#define EXPIRE  30 * 60
#define save_pic_path "http://47.103.210.8:8080/"//"http://192.168.204.132:8080/"
#define save_article_path "http://47.103.210.8:8080/"//"http://192.168.204.132:8080/"
#define handle_update_insert upd = tb.update().set("tel", Value(static_cast<std::string>(j["tel"]))). \
          set("address", Value(static_cast<std::string>(j["address"]))). \
          set("education", Value(static_cast<std::string>(j["education"]))). \
          set("name", Value(static_cast<std::string>(j["name"]))).  \
          set("email",  Value(static_cast<std::string>(j["email"]))). \
          set("experience", Value(static_cast<std::string>(j["experience"]))). \
          set("photo", Value(photo_field)). \
          set("isTeacher", Value(static_cast<int>(j["isTeacher"]))).


using json = nlohmann::json;
using namespace std;
using namespace ::mysqlx;
mysqlx::Client cli(mysqlx::SessionOption::USER, "root",
  mysqlx::SessionOption::PWD,  "root",
  mysqlx::SessionOption::HOST, "47.103.210.8",
  mysqlx::SessionOption::PORT, 33060,
  mysqlx::SessionOption::DB,   "lab",
  mysqlx::SessionOption::SSL_MODE, mysqlx::SSLMode::DISABLED,
  mysqlx::ClientOption::POOLING, true,
  mysqlx::ClientOption::POOL_MAX_SIZE, 100,
  mysqlx::ClientOption::POOL_QUEUE_TIMEOUT, 1000,
  mysqlx::ClientOption::POOL_MAX_IDLE_TIME, 500);

void handle_json_lab(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{
  try{
    Session mq = cli.getSession();
    res->addHeader("Content-Type","application/json;charset=utf-8");
    res->addHeader("Access-Control-Allow-Origin", "*");
    res->addHeader("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE, PATCH");
    res->addHeader("Access-Control-Max-Age", "3600");
    res->addHeader("Access-Control-Allow-Headers",
                  "Origin, X-Requested-With, Content-Type, Accept, token");
    auto sqlres = mq.sql("Select content FROM labimformation where type = 'labIntroduction'").execute();
    auto i = sqlres.fetchOne();
    std::string s_8 = smpHttp::Util::utf16Toutf8(i.get(0));
    json j;
    auto sqlres2 = mq.sql("Select content FROM labimformation where type = 'labDirection'").execute();
    auto i2 = sqlres2.fetchOne();
    std::string s2_8 = smpHttp::Util::utf16Toutf8(i2.get(0));
    vector<std::string> vec2 = smpHttp::Util::split(s2_8, ',');
    auto sqlres3 = mq.sql("Select content FROM labimformation where type = 'labImage'").execute();
    auto i3 = sqlres3.fetchOne();
    std::string s3_8 = smpHttp::Util::utf16Toutf8(i3.get(0));
    vector<std::string> vec3 = smpHttp::Util::split(s3_8, ',');
    j["labIntroduction"] = s_8;
    j["labDirection"] = vec2;
    j["labImage"] = vec3;
    j["membersData"] = json::object();
    json teachersData= json::array(),
      studentsData = json::array();
    auto tb = mq.getSchema("lab").getTable("labmembers");
    auto sqlres_member_date = tb.select("id", "photo", "name","education","experience","isTeacher").execute();
    Row member_row;
    while(member_row = sqlres_member_date.fetchOne()){
      json tj = json::object();
      tj["id"] = static_cast<int>(member_row.get(0));
      tj["photo"] = smpHttp::Util::utf16Toutf8(member_row.get(1));
      tj["name"] = smpHttp::Util::utf16Toutf8(member_row.get(2));
      tj["education"] = smpHttp::Util::utf16Toutf8(member_row.get(3));
      tj["experience"] = smpHttp::Util::utf16Toutf8(member_row.get(4));
   
      if(!member_row.get(5).isNull() && static_cast<int>(member_row.get(5))) 
        teachersData.push_back(tj);
      else 
        studentsData.push_back(tj);
    }
    j["membersData"]["teachersData"] = teachersData;
    j["membersData"]["studentsData"] = studentsData;
    std::string jso = j.dump();
    res->addMessage(jso);
  } catch(exception& e) {
    cout << "error: " << __LINE__ << " : " << __func__ << " : "<< e.what() << endl;
  }
}

void handle_json_news(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{
  try {
    Session mq = cli.getSession();
    res->addHeader("Content-Type","application/json;charset=utf-8");
    res->addHeader("Access-Control-Allow-Origin", "*");
    res->addHeader("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE, PATCH");
    res->addHeader("Access-Control-Max-Age", "3600");
    res->addHeader("Access-Control-Allow-Headers","Origin, X-Requested-With, Content-Type, Accept, token");
    auto sqlres = mq.sql("Select * FROM labnews").execute();  
    json resj;
    resj["news"] = json::array();
    Row member_row;
    while(member_row = sqlres.fetchOne()){
      json tj = json::object();
      tj["id"] = static_cast<int>(member_row.get(0));
      tj["newTitle"] = smpHttp::Util::utf16Toutf8(member_row.get(1));
      tj["newContent"] = smpHttp::Util::utf16Toutf8(member_row.get(2));
      tj["newAuthor"] = smpHttp::Util::utf16Toutf8(member_row.get(3));
      tj["newDate"] = smpHttp::Util::utf16Toutf8(member_row.get(4));
      tj["newImage"] = smpHttp::Util::utf16Toutf8(member_row.get(5));
      tj["isHot"] = static_cast<int>(member_row.get(6));
      resj["news"].push_back(tj);
    }
    std::string jso = resj.dump();
    res->addMessage(jso);
  } catch(exception& e) {
    cout << "error: " << __LINE__ << " : " << __func__ << " "<< e.what() << endl;
  }
}

void handle_base(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{
  res->addHeader("Access-Control-Allow-Origin", "*");
  res->addHeader("Access-Control-Allow-Methods", "POST, GET, PUT, OPTIONS, DELETE, PATCH");
  res->addHeader("Access-Control-Max-Age", "3600");
  res->addHeader("Access-Control-Allow-Headers","Origin, X-Requested-With, Content-Type, Accept, token");
  res->addHeader("Access-Control-Expose-Headers", "token");
  if(req->getHeader("token").size() && req->getMethod() == hpr::POST) {
    ExpValidator exp1;
    try{
      HS256Validator signer("secret");
      ::json header, payload;
      std::tie(header, payload) = JWT::Decode(req->getHeader("token"), &signer, &exp1);
      payload["exp"] = time(nullptr) + EXPIRE;
      std::string token = JWT::Encode(signer, payload);
      res->addHeader("token", token);
    }
    catch (TokenFormatError &tfe) {
      // res->setHttpStatus(smpHttp::HTTP_FORBIDDEN);
      res->addHeader("token", "11");
      cout << "error: " << tfe.what() << endl;
      res->end();
    }
    catch(InvalidClaimError& tfe){ //expired
      res->addHeader("token", "12");
      cout << "error: " << tfe.what() << endl;
      res->end();
    }
    catch (InvalidTokenError &tfe) {
      // res->setHttpStatus(smpHttp::HTTP_FORBIDDEN);
      json header, payload;
      std::tie(header, payload) = JWT::Decode(req->getHeader("token"));
      std::cout << "Payload: " << payload << std::endl;
      cout << "error: " << tfe.what() << endl;
      res->addHeader("token", "13");
      res->end();
    }
  } 
}

void handle_member_change(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{
  if(req->getMethod() != hpr::POST) {
    res->setHttpStatus(smpHttp::HTTP_METHOD_NOT_ALLOWED);
    res->addMessage("use post");
    res->end();
    return ;
  }
  res->addHeader("Content-Type","application/json;charset=utf-8");
  try {
    auto mq = cli.getSession();
    auto tb = mq.getSchema("lab").getTable("labmembers");
    json j = json::parse(req->getData());
    if(j["type"] == "delete") {
      json jres;
      std::string range = "(";
      for (auto& el : j["id"].items()) {
        range += to_string(static_cast<int>(el.value())) + ",";
      }
      range.back() = ')';
      auto res_remove = tb.remove().where(std::string("id in ") + range).execute();
      if(res_remove.getAffectedItemsCount() < 0) {
        throw runtime_error("delete nothing");
      }
      jres["status"] = "delete";
      res->addMessage(jres.dump());
    } else {
      uv_fs_t* req = new uv_fs_t();
      const std::string& str_photo = static_cast<std::string>(j["photo"]);
      std::string photo_field;
      if(smpHttp::Util::isBase64(str_photo)) {
        auto suffix = str_photo.find_first_of('/');
        auto suffix_end = str_photo.find_first_of(";");
        std::string save_path = "resources/pic" + to_string(static_cast<int>(j["id"])) + "." + str_photo.substr(suffix + 1, suffix_end - suffix - 1);
        photo_field = save_pic_path + save_path;
        const char* ss = str_photo.c_str() + str_photo.find_first_of(',') + 1;
        std::string* decode_res = smpHttp::Base64Decoder::base64_decode(ss, 1);
        req->data = decode_res;
        uv_fs_open(uv_default_loop(), req, save_path.c_str(), UV_FS_O_RDWR | UV_FS_O_CREAT | UV_FS_O_TRUNC
          , S_IRUSR | S_IRGRP | S_IROTH |S_IWGRP| S_IWUSR|S_IWOTH, [](uv_fs_t* req){
            int fd = req->result;
            if(fd < 0) {
              uv_fs_req_cleanup(req);
              delete req;
              return ;
            }
            std::string* decode_res = static_cast<std::string*>(req->data);
            uv_buf_t* buf = new uv_buf_t();
            buf->base = const_cast<char*>(decode_res->c_str());
            buf->len = decode_res->size();
            uv_fs_req_cleanup(req);
            buf_and_data* a = new buf_and_data({
              buf,
              decode_res,
              fd
            });
            req->data = a;
            uv_fs_write(uv_default_loop(), req, fd, buf, 1, 0, [](uv_fs_t* req){
              if(req->result < 0) {
                cout << "write error: " << uv_strerror(req->result) << endl; 
              }
              buf_and_data* a = static_cast<buf_and_data*>(req->data);
              delete a->str;
              delete a->buf;
              uv_fs_req_cleanup(req);
              uv_fs_close(uv_default_loop(), req, a->fd,  [](uv_fs_t* req){
                uv_fs_req_cleanup(req);
                delete req;
              });
              delete a;
            });
          });
      } else {
        photo_field = str_photo;
      }
      if(j["type"] == "update") {
        Result upd;
        if(!j["password"].is_null() && j["username"].is_null()) {
          handle_update_insert
          set("password", Value(static_cast<std::string>(j["password"]))).
          where(std::string("id=") + to_string(static_cast<int>(j["id"])) ).
          execute();
        } else if(!j["password"].is_null() && !j["username"].is_null()) {
          handle_update_insert
          set("password", Value(static_cast<std::string>(j["password"]))).
          set("username", Value(static_cast<std::string>(j["username"]))).
          where(std::string("id=") + to_string(static_cast<int>(j["id"])) ).
          execute();
        } else if (j["password"].is_null() && !j["username"].is_null()) {
          handle_update_insert
          set("username", Value(static_cast<std::string>(j["username"]))).
          where(std::string("id=") + to_string(static_cast<int>(j["id"])) ).
          execute();
        } else {
          handle_update_insert
          where(std::string("id=") + to_string(static_cast<int>(j["id"])) ).
          execute();
        }
        json jres;
        jres["status"] = "update";
        res->addMessage(jres.dump());
      } else if(j["type"] == "insert") {
        auto ind = tb.insert("tel","address","education","name","email","experience","photo","id", "sort","isTeacher","username","password").
          values(static_cast<std::string>(j["tel"]),
          static_cast<std::string>(j["address"]),
          static_cast<std::string>(j["education"]),
          static_cast<std::string>(j["name"]),
          static_cast<std::string>(j["email"]),
          static_cast<std::string>(j["experience"]),
          photo_field,
          static_cast<int>(j["id"]), 0,static_cast<int>(j["isTeacher"]),
          to_string(static_cast<int>(j["id"])),"123456").
          execute(); 
        json jres;
        jres["status"] = "insert";
        res->addMessage(jres.dump());
      }
    }
  } catch(exception& e) {
    cout << "error: handle_member_change : " << e.what() << endl;
    json jres;
    jres["status"] = "failed";
    jres["reason"] = e.what();
    res->addMessage(jres.dump());
    res->end();
    return ;
  } 
}

void handle_login(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{ 
  if(req->getMethod() != hpr::POST) {
    res->setHttpStatus(smpHttp::HTTP_METHOD_NOT_ALLOWED);
    res->addMessage("use post");
    res->end();
    return ;
  }
  try{
    json j = json::parse(req->getData());
    std::string username(j["username"]),
      password(j["password"]);
    auto mq = cli.getSession();
    auto tb = mq.getSchema("lab").getTable("labmembers");
    std::string condition = "username='"+username+"' AND "+"password='"+password + "'";
    auto sqlres = tb.select("name","sort", "id").where(std::move(condition)).limit(1).execute();
    res->addHeader("Content-Type","application/json;charset=utf-8");
    json rj = json::object();
    if(sqlres.count() == 0) {
      rj["status"] = false;
      res->addMessage(rj.dump());
      return ;
    }
    res->addHeader("Content-Type","application/json;charset=utf-8");
    Row r = sqlres.fetchOne();
    rj["status"] = true;
    rj["name"] = smpHttp::Util::utf16Toutf8(r.get(0));
    rj["sort"] = (bool)r.get(1);
    rj["id"] = static_cast<int>(r.get(2));
    rj["expire"] = time(nullptr) * 1000 + EXPIRE * 1000;
    time_t tim = time(nullptr);
    tim += EXPIRE;
    HS256Validator signer("secret");
    json payload = {{"exp", tim}, {"username", username}};
    std::string token = JWT::Encode(signer, payload);
    res->addHeader("token", token);
    res->addMessage(rj.dump());
  } catch(exception& e) {
    cout << "error: handle_login : " << e.what() << endl;
    // res->setHttpStatus(smpHttp::HTTP_FORBIDDEN);
    res->addMessage(e.what());
    res->end();
    return ;
  } 
}

void get_members(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) {
  if(req->getMethod() != hpr::GET) {
    res->setHttpStatus(smpHttp::HTTP_METHOD_NOT_ALLOWED);
    res->addMessage("use get");
    res->end();
    return ;
  }
  res->addHeader("Content-Type","application/json;charset=utf-8");
  auto mq = cli.getSession();
  auto tb = mq.getSchema("lab").getTable("labmembers");
  json j = {};
  j["members"] = json::array();
  try {
    if(req->getQuery("id").size()) {
      auto ress = tb.select("tel","address","education","name","email","experience","photo","id","isTeacher","username").
        where("id=" + req->getQuery("id") ).
        execute();
      vector<Row> rows = ress.fetchAll();
      for(auto i = rows.begin(); i < rows.end(); ++i) {
        json j_p = {};
        j_p["tel"] = smpHttp::Util::utf16Toutf8(i->get(0));
        j_p["address"] = smpHttp::Util::utf16Toutf8(i->get(1));
        j_p["education"] = smpHttp::Util::utf16Toutf8(i->get(2));
        j_p["name"] = smpHttp::Util::utf16Toutf8(i->get(3));
        j_p["email"] = smpHttp::Util::utf16Toutf8(i->get(4));
        j_p["experience"] = smpHttp::Util::utf16Toutf8(i->get(5));
        j_p["photo"] = smpHttp::Util::utf16Toutf8(i->get(6));
        j_p["id"] = static_cast<int>(i->get(7));
        j_p["isTeacher"] = static_cast<int>(i->get(8));
        j_p["username"] = smpHttp::Util::utf16Toutf8(i->get(9));
        j["members"].push_back(j_p);
      }
      res->addMessage(j.dump());
      return ;
    } else {
      auto tb_size = tb.count();
      j["members_total_size"] = tb_size;
      size_t req_page_size = req->getQuery("page_size").size() ? atoi(req->getQuery("page_size").c_str()) : 0;
      size_t req_page_num =  req->getQuery("page_num").size() ? atoi(req->getQuery("page_num").c_str()) : 0; 
      size_t start_n;
      if(!req_page_size || !req_page_num) { 
        start_n = 0;
        req_page_size = tb_size;
      } else {
        start_n = req_page_size * (req_page_num - 1) > tb_size ? tb_size : req_page_size * (req_page_num - 1);
        if(start_n == tb_size) {
          res->addMessage("out of range");
          res->end();
          return ;
        }
      }
      auto ress = tb.select("tel","address","education","name","email","experience","photo","id","isTeacher").
        orderBy("id").
        limit(req_page_size*req_page_num == 0 ? tb_size : req_page_size*req_page_num).
        execute();
      vector<Row> rows = ress.fetchAll();
      for(auto i = rows.begin() + start_n; i < rows.end(); ++i) {
        json j_p = {};
        j_p["tel"] = smpHttp::Util::utf16Toutf8(i->get(0));
        j_p["address"] = smpHttp::Util::utf16Toutf8(i->get(1));
        j_p["education"] = smpHttp::Util::utf16Toutf8(i->get(2));
        j_p["name"] = smpHttp::Util::utf16Toutf8(i->get(3));
        j_p["email"] = smpHttp::Util::utf16Toutf8(i->get(4));
        j_p["experience"] = smpHttp::Util::utf16Toutf8(i->get(5));
        j_p["photo"] = smpHttp::Util::utf16Toutf8(i->get(6));
        j_p["id"] = static_cast<int>(i->get(7));
        j_p["isTeacher"] = static_cast<int>(i->get(8));
        j["members"].push_back(j_p);
      }
      res->addMessage(j.dump());
      return ;
    }
  } catch(exception& e) {
    cout << "error: get_members : " << e.what() << endl;
    res->addMessage(e.what());
    res->end();
    return ;
  } 
}

void change_assets(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) {
  if(req->getMethod() != hpr::POST) {
    res->setHttpStatus(smpHttp::HTTP_METHOD_NOT_ALLOWED);
    res->addMessage("use post to change_assets");
    res->end();
    return ;
  }
  res->addHeader("Content-Type","application/json;charset=utf-8");
  try {
    auto mq = cli.getSession();
    auto tb = mq.getSchema("lab").getTable("labassets");
    json j = json::parse(req->getData());
    // bool has_one = static_cast<int>(j["id"]) > 0;
    if(j["type"] == "delete") {
      json jres;
      std::string range = "(";
      for (auto& el : j["id"].items()) {
        range += to_string(static_cast<int>(el.value())) + ",";
      }
      range.back() = ')';
      auto res_remove = tb.remove().where(std::string("id in ") + range).execute();
      if(res_remove.getAffectedItemsCount() < 0) {
        throw runtime_error("delete nothing");
      }
      jres["status"] = "delete";
      res->addMessage(jres.dump());
    } else {
      if(j["type"] == "update") {
        auto upd = tb.update().set("item", Value(static_cast<std::string>(j["item"]))).
          set("remark", Value(static_cast<std::string>(j["remark"]))).
          set("marker", Value(static_cast<std::string>(j["marker"]))).
          set("date", Value(static_cast<std::string>(j["date"]))).
          set("money",  Value(static_cast<double>(j["money"]))).
          where(std::string("id=") + to_string(static_cast<int>(j["id"]))).
          execute();
        json jres;
        jres["status"] = "update";
        res->addMessage(jres.dump());
      } else if(j["type"] == "insert"){
        auto ind = tb.insert("item","remark","marker","date","money").
          values(static_cast<std::string>(j["item"]),
          static_cast<std::string>(j["remark"]),
          static_cast<std::string>(j["marker"]),
          static_cast<std::string>(j["date"]),
          static_cast<double>(j["money"])).
          execute(); 
        json jres;
        jres["status"] = "insert";
        res->addMessage(jres.dump());
      }
    }
  } catch(exception& e) {
    cout << "error: handle_assets_change : " << e.what() << endl;
    json jres;
    jres["status"] = "failed";
    jres["reason"] = e.what();
    res->addMessage(jres.dump());
    res->end();
    return ;
  } 
}

void get_assets(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) {
  if(req->getMethod() != hpr::GET) {
    res->setHttpStatus(smpHttp::HTTP_METHOD_NOT_ALLOWED);
    res->addMessage("use get");
    res->end();
    return ;
  }
  res->addHeader("Content-Type","application/json;charset=utf-8");
  try {
    auto mq = cli.getSession();
    auto tb = mq.getSchema("lab").getTable("labassets");
    json j = {};
    RowResult ress;
    if(req->getQuery("id").size()) {
      ress = tb.select("id","item","remark","marker","date","money").
        where("id=" + req->getQuery("id")).
        execute();  
    } else {
      ress = tb.select("id","item","remark","marker","date","money").
        orderBy("id").
        execute();  
    }
    vector<Row> rows = ress.fetchAll();
    for(auto i = rows.begin(); i < rows.end(); ++i) {
      json j_p = {};
      j_p["id"] = static_cast<int>(i->get(0));
      j_p["item"] = smpHttp::Util::utf16Toutf8(i->get(1));
      j_p["remark"] = smpHttp::Util::utf16Toutf8(i->get(2));
      j_p["marker"] = smpHttp::Util::utf16Toutf8(i->get(3));
      j_p["date"] = smpHttp::Util::utf16Toutf8(i->get(4));
      j_p["money"] = static_cast<double>(i->get(5));
      j["assets"].push_back(j_p);
    }
    res->addMessage(j.dump());
  }catch(exception& e) {
    cout << "error: handle_assets_get: " << e.what() << endl;
    json jres;
    jres["status"] = "failed";
    jres["reason"] = e.what();
    res->addMessage(jres.dump());
    res->end();
    return ;
  } 
}

void change_articles(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) 
{
  if(req->getMethod() != hpr::POST) {
    res->setHttpStatus(smpHttp::HTTP_METHOD_NOT_ALLOWED);
    res->addMessage("use post");
    res->end();
    return ;
  }
  res->addHeader("Content-Type","application/json;charset=utf-8");
  try {
    auto mq = cli.getSession();
    auto tb = mq.getSchema("lab").getTable("labarticles");
    json j = json::parse(req->getData());
    if(j["type"] == "delete") {
      json jres;
      std::string range = "(";
      for (auto& el : j["id"].items()) {
        range += to_string(static_cast<int>(el.value())) + ",";
      }
      range.back() = ')';
      auto res_remove = tb.remove().where(std::string("id in ") + range).execute();
      if(res_remove.getAffectedItemsCount() < 0) {
        throw runtime_error("delete nothing");
      }
      jres["status"] = "delete";
      res->addMessage(jres.dump());
    } else {
      uv_fs_t* req = new uv_fs_t();
      int id;
      if(j["type"] == "update") {
        id = static_cast<int>(j["id"]);
      } else if(j["type"] == "insert"){
        auto res_id = mq.sql("select max(id) from labarticles").execute();
        id = static_cast<int>(res_id.fetchOne().get(0)) + 1;
      }
      std::string save_path = "resources/article_" + to_string(id);
      std::string* decode_res = new std::string(static_cast<std::string>(j["content"]));
      req->data = decode_res;
      uv_fs_open(uv_default_loop(), req, save_path.c_str(), UV_FS_O_RDWR | UV_FS_O_CREAT | UV_FS_O_TRUNC
        , S_IRUSR | S_IRGRP | S_IROTH |S_IWGRP| S_IWUSR|S_IWOTH, [](uv_fs_t* req){
          int fd = req->result;
          if(fd < 0) {
            uv_fs_req_cleanup(req);
            delete req;
            return ;
          }
          std::string* decode_res = static_cast<std::string*>(req->data);
          uv_buf_t* buf = new uv_buf_t();
          buf->base = const_cast<char*>(decode_res->c_str());
          buf->len = decode_res->size();
          uv_fs_req_cleanup(req);
          buf_and_data* a = new buf_and_data({
            buf,
            decode_res,
            fd
          });
          req->data = a;
          uv_fs_write(uv_default_loop(), req, fd, buf, 1, 0, [](uv_fs_t* req){
            if(req->result < 0) {
              cout << "write error: " << uv_strerror(req->result) << endl; 
            }
            buf_and_data* a = static_cast<buf_and_data*>(req->data);
            delete a->str;
            delete a->buf;
            uv_fs_req_cleanup(req);
            uv_fs_close(uv_default_loop(), req, a->fd,  [](uv_fs_t* req){
              uv_fs_req_cleanup(req);
              delete req;
            });
            delete a;
          });
        });
      if(j["type"] == "update") {
        auto upd = tb.update().set("title", Value(static_cast<std::string>(j["title"]))).
          set("isHot", Value(static_cast<int>(j["isHot"]))).
          set("isDraft", Value(static_cast<int>(j["isDraft"]))).
          set("content", Value(save_article_path + save_path)).
          set("author", Value(static_cast<std::string>(j["author"]))).
          set("date",Value(static_cast<std::string>(j["date"]))).
          where(std::string("id=") + to_string(static_cast<int>(j["id"]))).
          execute();
        json jres;
        jres["status"] = "update";
        res->addMessage(jres.dump());
      } else if(j["type"] == "insert") {
        auto ind = tb.insert("title","content","isHot","isDraft","author","date").
          values(static_cast<std::string>(j["title"]),
          save_article_path + save_path,
          static_cast<int>(j["isHot"]),
          static_cast<int>(j["isDraft"]),
          static_cast<std::string>(j["author"]),
          static_cast<std::string>(j["date"])
          ).
          execute(); 
        json jres;
        jres["status"] = "insert";
        res->addMessage(jres.dump());
      }
    }
  } catch(exception& e) {
    cout << "error: add_articles : " << e.what() << endl;
    json jres;
    jres["status"] = "failed";
    jres["reason"] = e.what();
    res->addMessage(jres.dump());
    res->end();
    return ;
  } 
}

void get_articles(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) {
  if(req->getMethod() != hpr::GET) {
    res->setHttpStatus(smpHttp::HTTP_METHOD_NOT_ALLOWED);
    res->addMessage("use get");
    res->end();
    return ;
  }
  res->addHeader("Content-Type","application/json;charset=utf-8");
  try {
    auto mq = cli.getSession();
    auto tb = mq.getSchema("lab").getTable("labarticles");
    json j = {};
    RowResult ress;
    if(req->getQuery("id").size()) {
      ress = tb.select("id","title","content","isHot","isDraft","author","date").
        where("id=" + req->getQuery("id")).
        execute();  
    } else if(req->getQuery("isHot").size()){
      ress = tb.select("id","title","content","isHot","isDraft","author","date").
        where("isHot="+req->getQuery("isHot")).
        orderBy("id").
        execute();  
    } else if(req->getQuery("isDraft").size()){
      ress = tb.select("id","title","content","isHot","isDraft","author","date").
        where("isDraft="+req->getQuery("isDraft")).
        orderBy("id").
        execute();  
    } else {
      ress = tb.select("id","title","content","isHot","isDraft","author","date").
        orderBy("id").
        execute();  
    }
    vector<Row> rows = ress.fetchAll();
    for(auto i = rows.begin(); i < rows.end(); ++i) {
      json j_p = {};
      j_p["id"] = static_cast<int>(i->get(0));
      j_p["title"] = smpHttp::Util::utf16Toutf8(i->get(1));
      j_p["content"] = smpHttp::Util::utf16Toutf8(i->get(2));
      j_p["isHot"] = static_cast<int>(i->get(3));
      j_p["isDraft"] = static_cast<bool>(i->get(4));
      j_p["author"] = static_cast<std::string>(i->get(5));
      j_p["date"] = static_cast<std::string>(i->get(6));
      j["articles"].push_back(j_p);
    }
    res->addMessage(j.dump());
  }catch(exception& e) {
    cout << "error: get_articles: " << e.what() << endl;
    json jres;
    jres["status"] = "failed";
    jres["reason"] = e.what();
    res->addMessage(jres.dump());
    res->end();
    return ;
  } 
}

void get_assets_inf(std::shared_ptr<smpHttp::HttpRequest> req
  , std::shared_ptr<smpHttp::HttpResponse> res) {
  if(req->getMethod() != hpr::GET) {
    res->setHttpStatus(smpHttp::HTTP_METHOD_NOT_ALLOWED);
    res->addMessage("use get");
    res->end();
    return ;
  }
  res->addHeader("Content-Type","application/json;charset=utf-8");
  try {
    auto mq = cli.getSession();
    auto tb = mq.sql("select item, COUNT(*) AS number, SUM(money) AS money from labassets GROUP BY item").execute();
    Row r;
    json j = {};
    while(r = tb.fetchOne()) {
      j["numbers"][smpHttp::Util::utf16Toutf8(r.get(0))] = static_cast<int>(r.get(1));
      j["moneys"][smpHttp::Util::utf16Toutf8(r.get(0))] = static_cast<double>(r.get(2));
    }
    res->addMessage(j.dump());
  }catch(exception& e) {
    cout << "error: get_assets_inf: " << e.what() << endl;
    json jres;
    jres["status"] = "failed";
    jres["reason"] = e.what();
    res->addMessage(jres.dump());
    res->end();
    return ;
  } 
}