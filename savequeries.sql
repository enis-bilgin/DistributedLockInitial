/*simple create table*/
CREATE TABLE distributed_dev (
  topic_key serial primary key,
  topic_name varchar(10) not null,
  write_access boolean not null,
  primary_up boolean not null,
  expiration_time timestamp default current_timestamp
)


SELECT * FROM distributed_dev;

/*simple test insert*/
INSERT INTO distributed_dev (topic_name,write_access,primary_up,expiration_time)
VALUES ('S3T1',true,false,current_timestamp);


/*Upsert*/
INSERT INTO distributed_dev (topic_name,write_access,primary_up,expiration_time)
VALUES ($1,$2,$3,4$);

SELECT * FROM distributed_dev;