#ifndef DBPSQL_STTMNTS_HPP
#define DBPSQL_STTMNTS_HPP




namespace psqldb {


    /**
     * TABLE NAME INFO = info_distributed_dev
     * TABLE NAME = distributed_dev
     * topic_key generated automatically, this will be our UNIQUE key id (PRIMARY)
     * topic_name must be UNIQUE (CONSTAINT), inserting same name not possible
     * */

    // create info table if not exists
    static const char * CREATEINFOTABLE =
            "CREATE TABLE IF NOT EXISTS info_distributed_dev (" \
            "  proc_num serial primary key," \
            "  proc_name varchar(20) not null unique," \
            "  lock_id int not null," \
            "  num_topics int not null," \
            "  primary_topics VARCHAR(40) not null," \
            "  update_time timestamp default current_timestamp" \
            ");";



    // create topic table if not exists
    static const char * CREATETOPICTABLE =
            "CREATE TABLE IF NOT EXISTS distributed_dev (" \
            "  lock_id int not null unique primary key," \
            "  topic_name varchar(10) not null unique," \
            "  write_access boolean not null," \
            "  read_access boolean not null,"
            "  primary_up boolean not null," \
            "  expiration_time timestamp default current_timestamp" \
            ");";

    // delete table
    static const char * DROPTOPICTABLE = "DROP TABLE distributed_dev";
    static const char * DROPINFOTABLE = "DROP TABLE info_distributed_dev";


    // Prepare Statements
    static const char * PREPINSERTINFO =
            "INSERT INTO info_distributed_dev (" \
            "proc_name,lock_id,num_topics,primary_topics) "
            "VALUES ($1,$2,$3,$4)" \
            "ON CONFLICT (proc_name) DO UPDATE " \
            "SET lock_id=$2, num_topics=$3, primary_topics=$4, update_time=current_timestamp";


    // Prepare Statements Insert Into Lock Table
    static const char * PREPINSERTTOPIC =
            "INSERT INTO distributed_dev (" \
            "lock_id,topic_name,write_access,read_access,primary_up) "
            "VALUES ($1,$2,$3,$4,$5)" \
            "ON CONFLICT (topic_name) DO UPDATE "
            "SET expiration_time=current_timestamp";


    // lock unlock with KEY
    static const char* PREPLOCK   = "SELECT pg_advisory_lock($1)";
    static const char* PREPUNLOCK = "SELECT pg_advisory_unlock($1)";

}

#endif