#ifndef DBPSQL_STTMNTS_HPP
#define DBPSQL_STTMNTS_HPP




namespace psqldb {

    /**
     *
     * 2 TABLE (INFO,LOCK)
     * TABLE NAME INFO = info_recorders --> info table
     * TABLE NAME = lock_recorders --/ Lock Table for recorders
     * topic_key generated automatically, this will be our UNIQUE key id (PRIMARY)
     * topic_name must be UNIQUE (CONSTRAINT), inserting the same name is not possible
     *
     **/
    // create info table if not exists
    static const char * CREATEINFOTABLE =
            "CREATE TABLE IF NOT EXISTS info_recorders (" \
            "  proc_num serial primary key," \
            "  proc_name varchar(20) not null unique," \
            "  num_topics int not null," \
            "  primary_recorders varchar(255) not null," \
            "  secondary_recorders varchar(255) not null," \
            "  update_time timestamp default current_timestamp" \
            ");";


    // create recordertest table if not exists
    static const char * CREATETOPICTABLE =
            "CREATE TABLE IF NOT EXISTS lock_recorders (" \
            "  lock_id int not null unique primary key," \
            "  topic_name varchar(20) not null unique," \
            "  write_access boolean not null," \
            "  read_access boolean not null,"
            "  primary_up boolean not null," \
            "  update_time timestamp default current_timestamp" \
            ");";

    // delete table
    static const char * DROPLOCKTABLE = "DROP TABLE lock_recorders";
    static const char * DROPINFOTABLE = "DROP TABLE info_recorders";


    // Prepare Statements
    static const char * PREPINSERTINFO =
            "INSERT INTO info_recorders (" \
            "proc_name,num_topics,primary_recorders,secondary_recorders) "
            "VALUES ($1,$2,$3,$4)" \
            "ON CONFLICT (proc_name) DO UPDATE " \
            "SET num_topics=$2, primary_recorders=$3, secondary_recorders=$4, update_time=current_timestamp";


    // Prepare Statements Insert Into Lock Table
    static const char * PREPINSERTRECORDER =
            "INSERT INTO lock_recorders (" \
            "lock_id,topic_name,write_access,read_access,primary_up) "
            "VALUES ($1,$2,$3,$4,$5)" \
            "ON CONFLICT (lock_id) DO UPDATE "
            "SET topic_name=$2, primary_up=$5, update_time=current_timestamp";

    static const char * PREPUPDATEDBYLOCKID =
            "UPDATE lock_recorders " \
            "SET update_time=current_timestamp " \
            "WHERE lock_id=$1";


    static const char * PREPUPDATEPRIMARYUP =
            "UPDATE lock_recorders " \
            "SET primary_up=$2, update_time=current_timestamp " \
            "WHERE lock_id=$1";


    // get time
    static const char * PREPTIMEDIFFMICROS = "SELECT (EXTRACT(epoch FROM current_timestamp)*1000000) " \
                                             "- (EXTRACT(epoch FROM update_time)*1000000) " \
                                             "FROM lock_recorders WHERE lock_id=$1";

    // bool
    static const char * PREPWRITEACCESS = "SELECT write_access FROM lock_recorders WHERE lock_id=$1";
    static const char * PREPREADACCESS  = "SELECT read_access FROM lock_recorders WHERE lock_id=$1";
    static const char * PREPISPRIMARYUP = "SELECT primary_up FROM lock_recorders WHERE lock_id=$1";


    // lock unlock with KEY
    static const char* PREPLOCK   = "SELECT pg_advisory_lock($1)";
    static const char* PREPUNLOCK = "SELECT pg_advisory_unlock($1)";

}

#endif