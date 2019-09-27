/*
 * Copyright (c) 2015, 2019, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef CDK_MYSQLX_SESSION_H
#define CDK_MYSQLX_SESSION_H

#include <mysql/cdk/data_source.h>
#include <mysql/cdk/codec.h>
#include <mysql/cdk/protocol/mysqlx/collations.h>

#include "common.h"

PUSH_SYS_WARNINGS_CDK
#include <deque>
POP_SYS_WARNINGS_CDK

#undef max
using cdk::shared_ptr;

// TODO: CS handling
#define BINARY_CS_ID 63


struct cdk::Format<cdk::TYPE_INTEGER>::Access
{
  typedef cdk::Format<cdk::TYPE_INTEGER> Format;
  static void set_fmt(Format &o, Format::Fmt fmt) { o.m_fmt= fmt; }
  static void set_length(Format &o, size_t len)   { o.m_length= len; }
};


struct cdk::Format<cdk::TYPE_FLOAT>::Access
{
  typedef cdk::Format<cdk::TYPE_FLOAT> Format;
  static void set_fmt(Format &o, Format::Fmt fmt) { o.m_fmt= fmt; }
};


struct cdk::Format<cdk::TYPE_BYTES>::Access
{
  typedef cdk::Format<cdk::TYPE_BYTES> Format;
  static void set_width(Format &o, uint64_t width) { o.m_width= width; }
};


struct cdk::Format<cdk::TYPE_STRING>::Access
{
  typedef cdk::Format<cdk::TYPE_STRING> Format;
  static void set_width(Format &o, uint64_t width) { o.m_width= width; }
  static void set_cs(Format &o, Charset::value cs) { o.m_cs= cs; }
  static void set_kind_set(Format &o) { o.m_kind= Format::SET; }
  static void set_kind_enum(Format &o) { o.m_kind= Format::ENUM; }
};


struct cdk::Format<cdk::TYPE_DATETIME>::Access
{
  //typedef cdk::Format<cdk::TYPE_FLOAT> Format;
  static void set_fmt(Format &o, Format::Fmt fmt, bool has_time)
  {
    o.m_fmt = fmt;
    o.m_has_time = has_time;
  }
};


namespace cdk {
namespace mysqlx {

class Session;
class Cursor;


// ---------------------------------------------------------

/*
  Classes to store meta-data information received from server.
*/


template <class Base>
class Obj_ref : public Base
{
protected:

  string m_name;
  string m_name_original;
  bool   m_has_name_original;

public:

  Obj_ref()
    : m_has_name_original(false)
  {}

  Obj_ref(const cdk::api::Ref_base &ref)
    : m_name(ref.name())
    , m_name_original(ref.orig_name())
    , m_has_name_original(true)
  {}

  const string name() const { return m_name; }
  const string orig_name() const
  {
    return m_has_name_original ? m_name_original : m_name;
  }

  friend class Session;
};


/*
  Determine charset from collation id reported by the protocol. The mapping
  is given by COLLATIONS_XXX() lists in collations.h.
*/

inline
cdk::Charset::value get_collation_cs(collation_id_t id)
{
  /*
    If collation id is 0, that is, there is no collation info in server
    reply, we assume utf8.
  */

  if (0 == id)
    return cdk::Charset::utf8;

#undef  CS
#define COLL_TO_CS(CS) COLLATIONS_##CS(COLL_TO_CS_CASE) return cdk::Charset::CS;
#define COLL_TO_CS_CASE(CS,ID,COLL,CC)  case ID:

  switch (id)
  {
    CDK_CS_LIST(COLL_TO_CS)
  default:
    THROW("Unkonwn collation id");
  }
}


class Col_metadata
  : public Obj_ref<cdk::Column_info>
  , public cdk::Format_info
{
  typedef Column_info::length_t length_t;

  int          m_type;
  int          m_content_type;
  length_t     m_length;
  unsigned int m_decimals;
  collation_id_t m_cs;
  uint32_t     m_flags;

  class : public Obj_ref<cdk::api::Table_ref>
  {
    class : public Obj_ref<cdk::api::Schema_ref>
    {
      Obj_ref<cdk::api::Ref_base> m_catalog;
      const cdk::api::Ref_base* catalog() const { return &m_catalog; }
      friend class Session;
    } m_schema;

    bool m_has_schema;

    const cdk::api::Schema_ref* schema() const
    {
      return m_has_schema ? &m_schema : nullptr;
    }

    friend class Session;
  } m_table;

  bool      m_has_table;

  const cdk::api::Table_ref* table() const
  {
    return m_has_table ? &m_table : nullptr;
  }

  /*
    Format_info interface
    ---------------------
  */

  bool for_type(Type_info type) const
  {
    switch (m_type)
    {
    case protocol::mysqlx::col_type::SINT:
    case protocol::mysqlx::col_type::UINT:
      return TYPE_INTEGER == type;

    case protocol::mysqlx::col_type::FLOAT:
    case protocol::mysqlx::col_type::DOUBLE:
    case protocol::mysqlx::col_type::DECIMAL:
      return TYPE_FLOAT == type;

    case protocol::mysqlx::col_type::TIME:
    case protocol::mysqlx::col_type::DATETIME:
      return TYPE_DATETIME == type;

    case protocol::mysqlx::col_type::BYTES:
      switch (m_content_type)
      {
      case content_type::JSON: return TYPE_DOCUMENT == type;
      case content_type::GEOMETRY: return TYPE_GEOMETRY == type;
      case content_type::XML: return TYPE_XML == type;
      default: break;
      }

      FALLTHROUGH;
    case protocol::mysqlx::col_type::ENUM:
    default:
      return TYPE_BYTES == type || TYPE_STRING == type;
    }
  }

  /*
    Methods get_info() update a Type_info object to describe the
    encoding format used by this column data.
  */

  void get_info(Format<TYPE_INTEGER>& fmt) const
  {
    switch (m_type)
    {
    case protocol::mysqlx::col_type::SINT:
      Format<TYPE_INTEGER>::Access::set_fmt(fmt, Format<TYPE_INTEGER>::SINT);
      break;
    case protocol::mysqlx::col_type::UINT:
      Format<TYPE_INTEGER>::Access::set_fmt(fmt, Format<TYPE_INTEGER>::UINT);
      break;
    }
    Format<TYPE_INTEGER>::Access::set_length(fmt, m_length);
  }

  void get_info(Format<TYPE_FLOAT>& fmt) const
  {
    switch (m_type)
    {
    case protocol::mysqlx::col_type::FLOAT:
      Format<TYPE_FLOAT>::Access::set_fmt(fmt, Format<TYPE_FLOAT>::FLOAT);
      break;
    case protocol::mysqlx::col_type::DOUBLE:
      Format<TYPE_FLOAT>::Access::set_fmt(fmt, Format<TYPE_FLOAT>::DOUBLE);
      break;
    case protocol::mysqlx::col_type::DECIMAL:
      Format<TYPE_FLOAT>::Access::set_fmt(fmt, Format<TYPE_FLOAT>::DECIMAL);
      break;
    }
  }

  void get_info(Format<TYPE_STRING>& fmt) const
  {
    Format<TYPE_STRING>::Access::set_cs(fmt, get_collation_cs(m_cs));

    /*
      Note: Types ENUM and SET are generally treated as
      strings, but we set a 'kind' flag in the format description
      to be able to distinguish them from plain strings.
    */

    switch (m_type)
    {
    case protocol::mysqlx::col_type::BYTES:
      Format<TYPE_STRING>::Access::set_width(fmt, m_length);
      break;
    case protocol::mysqlx::col_type::SET:
      Format<TYPE_STRING>::Access::set_kind_set(fmt);
      break;
    case protocol::mysqlx::col_type::ENUM:
      Format<TYPE_STRING>::Access::set_kind_enum(fmt);
      break;
    }
  }

  void get_info(Format<TYPE_DATETIME>& fmt) const
  {
    switch (m_type)
    {
    case protocol::mysqlx::col_type::TIME:
      Format<TYPE_DATETIME>::Access::set_fmt(fmt, Format<TYPE_DATETIME>::TIME, true);
      break;

    case protocol::mysqlx::col_type::DATETIME:

      // Note: flag 0x01 distinguishes TIMESTAMP from DATETIME type.

      if (m_flags & 0x01)
        Format<TYPE_DATETIME>::Access::set_fmt(fmt,
          Format<TYPE_DATETIME>::TIMESTAMP, true);
      else
      {
        /*
        Note: presence of time part is detected based on the length
        of the column. Full DATETIME values occupy more than 10
        positions.
        */

        Format<TYPE_DATETIME>::Access::set_fmt(fmt,
          Format<TYPE_DATETIME>::DATETIME, m_length > 10);
      }
      break;
    }
  }

  void get_info(Format<TYPE_BYTES> &fmt) const
  {
    // Note: flag 0x01 means that bytes should be padded with 0x00

    if (m_flags & 0x01)
    {
      Format<TYPE_BYTES>::Access::set_width(fmt, m_length);
    }
  }

  /*
    Note: Access to default implementation for all overloads that
    are not explicitly defined above
    (see: http://stackoverflow.com/questions/9995421/gcc-woverloaded-virtual-warnings
  */

  using Format_info::get_info;

public:

  Col_metadata()
    : m_type(0)
    , m_content_type(0)
    , m_length(0)
    , m_decimals(0)
    , m_cs(BINARY_CS_ID)
    , m_flags(0)
    , m_has_table(false)
  {}

  length_t length() const { return m_length; }
  length_t decimals() const { return m_decimals; }
  collation_id_t collation() const { return m_cs; }

  friend class Session;
  friend class Cursor;
};


typedef std::map<col_count_t, Col_metadata>  Mdata_storage;

// ---------------------------------------------------------

/*
  Note: other Session implementations might need to translate genric
  cdk types to something that is specific to the implementation.
*/

using cdk::Row_source;
using cdk::Projection;
using cdk::Limit;
using cdk::Order_by;
using cdk::Sort_direction;
using cdk::Param_source;
using cdk::View_spec;

typedef Session Reply_init;

class Reply;
class Cursor;
class Proto_prepare_op;

class SessionAuthInterface
{
public:

  virtual ~SessionAuthInterface() {}

  virtual const char* auth_method() = 0;
  virtual bytes auth_data()  = 0;
  virtual bytes auth_response() = 0;
  virtual bytes auth_continue(bytes) = 0;
};


typedef protocol::mysqlx::api::Protocol_fields Protocol_fields;

class Session
    : public api::Diagnostics
    , public Async_op
    , private protocol::mysqlx::Auth_processor
    , private protocol::mysqlx::Mdata_processor
    , private protocol::mysqlx::Stmt_processor
    , private protocol::mysqlx::SessionState_processor
{

  friend class Reply;
  friend class Cursor;

  struct Doc_args : public Any_list
  {
    const cdk::Any::Document *m_doc;
    void process(Processor &prc) const;
  };

  struct Prepare_processor
      : public Reply_processor
  {
    Session *m_session = nullptr;

    virtual void error(unsigned int code, short int severity,
                       sql_state_t sql_state, const string &msg);

    virtual void notice(unsigned int type,
                        short int scope,
                        bytes payload)
    {
      assert(m_session);
      m_session->notice(type, scope, payload);
    }

  };

  friend Prepare_processor;


protected:

  Protocol  m_protocol;

  bool      m_prepare_prepare = false;


  option_t  m_isvalid;
  Diagnostic_arena m_da;

  Reply* m_current_reply;

  scoped_ptr<SessionAuthInterface> m_auth_interface;

  shared_ptr<Proto_prepare_op> m_cmd;

  enum { CMD_SQL, CMD_ADMIN, CMD_COLL_ADD } m_cmd_type;

  string m_stmt;
  Doc_args m_cmd_args;
  const Table_ref *m_table;

  unsigned long m_id;
  bool m_expired;
  string m_cur_schema;
  uint64_t m_proto_fields = UINT64_MAX;

  struct
  {
    row_count_t  last_insert_id;
    row_count_t  rows_affected;
    row_count_t  rows_found;
    row_count_t  rows_matched;

    void clear()
    {
      last_insert_id = 0;
      rows_affected = 0;
      rows_found = 0;
      rows_matched = 0;
    }
  }
  m_stmt_stats;

  std::deque< shared_ptr<Proto_op> > m_op_queue;
  std::deque< shared_ptr<Proto_op> > m_reply_op_queue;
  Cursor*                 m_current_cursor;

  Prepare_processor m_prepare_prc;

  bool m_executed;
  bool m_has_results;
  bool m_discard;

public:

  //cdk::api::Connection* get_connection();

  typedef ds::Options<ds::mysqlx::Protocol_options> Options;

  template <class C>
  Session(C &conn, const Options &options)
    : m_protocol(conn)
    , m_isvalid(false)
    , m_current_reply(nullptr)
    , m_auth_interface(nullptr)
    , m_table(nullptr)
    , m_id(0)
    , m_expired(false)
    , m_current_cursor(nullptr)
    , m_executed(false)
    , m_has_results(false)
    , m_discard(false)
    , m_nr_cols(0)
  {
    m_prepare_prc.m_session = this;
    m_stmt_stats.clear();
    send_connection_attr(options);
    authenticate(options, conn.is_secure());

    // TODO: make "lazy" checks instead, deferring to the time when given
    // feature is used.
    check_protocol_fields();
  }

  virtual ~Session();

  /*
    Check if given session is valid. Function is_valid() performs
    a lightweight, local check while check_valid() might communicate with
    the data store to perform this check. Both is_valid() and check_valid()
    return UNKNOWN if session state could not be determined.
  */

  option_t is_valid();
  option_t check_valid();

  /*
    Check that xplugin is supporting certain new fields in the protocol
    such as row locking, etc. The function sets binary flags in
    m_proto_fields member variable
  */
  void check_protocol_fields();
  bool has_prepared_statements();
  void set_has_prepared_statements(bool);
  bool has_keep_open();

  /*
    Clear diagnostic information that accumulated for the session.
    Diagnostics interface methods such as Diagnostics::error_count()
    and Diagnostics::get_errors() report only new diagnostics entries
    since last call to clear_errors() (or since session creation if
    clear_errors() was not called).
  */
  void clear_errors()
  { m_da.clear(); }

  void reset();
  void close();

  /*
    Transactions
  */

  void begin();
  void commit();
  void rollback(const string &savepoint);
  void savepoint_set(const string &savepoint);
  void savepoint_remove(const string &savepoint);

  /*
    Prepared Statments
  */

  Reply_init& prepared_execute(uint32_t stmt_id,
                               const Limit *lim,
                               const Param_source *param
                               );
  Reply_init& prepared_execute(uint32_t stmt_id,
                               const cdk::Any_list *list
                               );

  Reply_init& prepared_deallocate(uint32_t stmt_id);

  /*
     SQL API
  */

  Reply_init &sql(uint32_t stmt_id, const string&, Any_list*);

  Reply_init &admin(const char*, const cdk::Any::Document&);

  /*
    CRUD API
  */

  Reply_init &coll_add(const Table_ref&,
                       Doc_source&,
                       const Param_source *param = nullptr,
                       bool upsert = false);

  Reply_init &coll_remove(uint32_t stmt_id,
                          const Table_ref&,
                          const Expression *expr = nullptr,
                          const Order_by *order_by = nullptr,
                          const Limit *lim = nullptr,
                          const Param_source *param = nullptr);
  Reply_init &coll_find(uint32_t stmt_id,
                        const Table_ref&,
                        const View_spec *view = nullptr,
                        const Expression *expr = nullptr,
                        const Expression::Document *proj = nullptr,
                        const Order_by *order_by = nullptr,
                        const Expr_list *group_by = nullptr,
                        const Expression *having = nullptr,
                        const Limit *lim = nullptr,
                        const Param_source *param = nullptr,
                        const Lock_mode_value lock_mode = Lock_mode_value::NONE,
                        const Lock_contention_value lock_contention
                          = Lock_contention_value::DEFAULT);
  Reply_init &coll_update(uint32_t stmt_id,
                          const api::Table_ref&,
                          const Expression*,
                          const Update_spec&,
                          const Order_by *order_by = nullptr,
                          const Limit* = nullptr,
                          const Param_source * = nullptr);

  Reply_init &table_delete(uint32_t stmt_id,
                           const Table_ref&,
                           const Expression *expr = nullptr,
                           const Order_by *order_by = nullptr,
                           const Limit *lim = nullptr,
                           const Param_source *param = nullptr);
  Reply_init &table_select(uint32_t stmt_id,
                           const Table_ref&,
                           const View_spec *view = nullptr,
                           const Expression *expr = nullptr,
                           const Projection *proj = nullptr,
                           const Order_by *order_by = nullptr,
                           const Expr_list *group_by = nullptr,
                           const Expression *having = nullptr,
                           const Limit *lim = nullptr,
                           const Param_source *param = nullptr,
                           const Lock_mode_value lock_mode = Lock_mode_value::NONE,
                           const Lock_contention_value lock_contention = Lock_contention_value::DEFAULT);
  Reply_init &table_insert(uint32_t stmt_id,
                           const Table_ref&,
                           Row_source&,
                           const api::Columns *cols,
                           const Param_source *param = nullptr);
  Reply_init &table_update(uint32_t stmt_id,
                           const api::Table_ref &coll,
                           const Expression *expr,
                           const Update_spec &us,
                           const Order_by *order_by = nullptr,
                           const Limit *lim = nullptr,
                           const Param_source *param = nullptr);

  Reply_init &view_drop(const api::Table_ref&, bool check_existence = false);



  /*
      Async (cdk::api::Async_op)
  */

  bool is_completed() const;
  const api::Event_info* get_event_info() const;


  // Diagnostics API

  unsigned int entry_count(Severity::value level=Severity::ERROR)
  { return m_da.entry_count(level); }

  Iterator& get_entries(Severity::value level=Severity::ERROR)
  { return m_da.get_entries(level); }

  const Error& get_error()
  { return m_da.get_error(); }


  const string& get_current_schema() const
  {
    return m_cur_schema;
  }

private:

  Reply_init & set_command(Proto_prepare_op *cmd);

  // Send Connection Attributes
  void send_connection_attr(const Options &options);
  // Authentication (cdk::protocol::mysqlx::Auth_processor)
  void authenticate(const Options &options, bool secure = false);
  void do_authenticate(const Options &options, int auth_method, bool secure);
  void send_auth();
  void auth_ok(bytes data);
  void auth_continue(bytes data);
  void auth_fail(bytes data);
  void error(unsigned int code, short int severity, sql_state_t sql_state, const string& msg);

  void add_diagnostics(Severity::value level, error_code code,
                       const string &msg = string());

  void add_diagnostics(Severity::value level, unsigned code,
                       sql_state_t sql_state,
                       const string &msg =string());
  void add_diagnostics(Severity::value level, const Error *e,
                       bool report_to_reply = true);

  //  Reply registration
  virtual void register_reply(Reply* reply);
  virtual void deregister_reply(Reply*);

  /*
     Mdata_processor (cdk::protocol::mysqlx::Mdata_processor)
  */

  void ok(string);
  void col_count(col_count_t nr_cols);
  void col_type(col_count_t pos, unsigned short type);
  void col_content_type(col_count_t pos, unsigned short type);
  void col_name(col_count_t pos,
                const string &name, const string &original);
  void col_table(col_count_t pos,
                 const string &table, const string &original);
  void col_schema(col_count_t pos,
                  const string &schema, const string &catalog);
  void col_collation(col_count_t pos, collation_id_t cs);
  void col_length(col_count_t pos, uint32_t length);
  void col_decimals(col_count_t pos, unsigned short decimals);
  void col_flags(col_count_t, uint32_t);


  /*
     Stmt_processor
  */

  void execute_ok();
  void notice(unsigned int /*type*/, short int /*scope*/, bytes /*payload*/);

  /*
    SessionState_processor
  */

  void client_id(unsigned long);
  void account_expired();
  void current_schema(const string&);
  void row_stats(row_stats_t, row_count_t);
  void last_insert_id(insert_id_t);
  // TODO: void trx_event(trx_event_t);
  void generated_document_id(const std::string&);

  /*
     Helper functions to send/receive protocol messages
  */

  void send_cmd();
  void start_reading_result();
  Proto_op* start_reading_row_data(protocol::mysqlx::Row_processor &prc);
  void start_reading_stmt_reply();
  void start_authentication(const char* mechanism,bytes data,bytes response);
  void start_authentication_continue(bytes data);
  void start_reading_auth_reply();

  /*
      Async (cdk::api::Async_op)
  */

  bool do_cont();
  void do_wait();
  void do_cancel();


private:

  // Meta data storage

  cdk::scoped_ptr<Mdata_storage> m_col_metadata;
  col_count_t m_nr_cols;

};


using cdk::api::Column_ref;
using cdk::api::Table_ref;
using cdk::api::Schema_ref;
using cdk::api::Object_ref;


}} //cdk::mysqlx



#endif // CDK_MYSQLX_SESSION_H
