#	Skeleton script for creating a table
#
#	Notes:
#	- Script should be formatted exactly as indicated
#	- The primary key columns should always be the first columns indicated
#	- All SQL statements should be terminated with a /
#	- Primary key, foreign key, and unique constraints are out-of-line.
#	- Multi-column constraints are out-of-line.
#	- All other constraints are in-line.
#	- Remove all comments above the next line
#=============================================================================
# Name      :
# Created On:
# Created By:
# SCN No    :
# Purpose   :
#==============================================================================
drop table owner.table_name
/
create table owner.table_name (
	column_name	varchar2(4)	constraint table_name_nn1 not null,
	column_name	varchar2(10),
	column_name	number(8,2) 	constraint 
					table_name_ck1 ( column_name > 0),
	column_name	number(4,2)	default 0,
	column_name	date,
	.
	.
	.
	constraint table_name_pk1
		primary key (column_name, column_name, ...)
#
# All constraints that create an index must have a USING INDEX clause
# To calculate the initial extent of an index, use the following (assuming
# a 4K block size):
# bytes = ((x*(11+x+len))/((4096 - 90)*(1 - pctfree/100)))*(4096 * 1.1)
# where
#	x 		= expected number of rows after 6 months
#	z		= number of columns indexed
#	len		= the average length of the indexed column
#
# Alternatively, you may use one of the following storage clauses:
#
#	Small (small indexes 20K to 2560K)
#
# storage (initial 20K  next 20K minextents 1 maxextents 99 pctincrease 100
#		freelists 1)
#
#	Medium (medium indexes 2560K to 40960K)
#
# storage (initial 2560K  next 20K minextents 1 maxextents 99 pctincrease 100
#		freelists 1)
#
#	Large (large indexes 40960K and up)
#
# storage (initial 40960K  next 320K minextents 1 maxextents 99 pctincrease 100
#		freelists 1)
#
		using index
			tablespace get_from_dba
			pctfree 10
			storage (initial 20K
				  next 20K
				  minextents 1 
				  maxextents 99
				  pctincrease 100
				  freelists 1)
	constraint table_name_fk1
		foreign key (column_name) references
		table_name (column_name),
	constraint table_name_u1
		unique (column_name, column_name)
		using index
			tablespace get_from_dba
			pctfree 10
			storage (initial 20K
				  next 20K
				  minextents 1 
				  maxextents 99
				  pctincrease 100
				  freelists 1)
# pctfree should be set to 0 for tables whose rows are not updated 
# (e.g. validation tables, control tables, history tables, etc.)
# and whose columsn will all be complete (not null) upon first insert.
#
# pctfree should be set higher than 10 for those rows where many columns
# will initially be left null upon the first insert, but will be updated
# later
pctfree 10 pctused 40
tablespace get_from_dba
#
# STORAGE CLAUSE
#
# Initial Extent
# Initial extent storage should be set to the appropriate number of bytes
# (rounded to a multiple of 20K bytes) this table is expected to grow to in 
# the first 6 months.
# To calculate the initial extent, use the following (assuming
# a 4K block size):
# bytes = ((x*(5+y*(1+len)))/((4096 - 90)*(1 - pctfree/100)))*4096
# where
#	x 		= expected number of rows after 6 months
#	y		= number of columns in the table
#	len		= the average column length
#	pctfree		= obtained from guidelines above
#
# Round the initial extent up to one of the values in the table below.
# Determine the next extent from the table below (again assuming 4K 
# block sizes):
#
#	Initial Extent		Next Extent
#	=============		===========
#	     20K		   20K
#	     40K		   20K
#	     80K		   20K
#	    160K		   20K
#	    320K		   20K
#	    640K		   20K
#	   1280K		   20K
#	   2560K		   20K
#	   5120K		   40K
#	  10240K		   80K
#	  20480K		  160K
#	  40960K		  320K
#	  81920K		  640K
#	 163840K		 1280K
#	 327680K		 2560K
#	 655360K		 5120K
#	1310720K		10240K
#	etc, with each succeeding value twice the previous value.
#
# Alternatively, you may use one of the following storage clauses:
#
#	Small (small tables 20K to 2560K)
#
# storage (initial 20K  next 20K minextents 1 maxextents 99 pctincrease 100
#		freelists 1)
#
#	Medium (medium tables 2560K to 40960K)
#
# storage (initial 2560K  next 20K minextents 1 maxextents 99 pctincrease 100
#		freelists 1)
#
#	Large (large tables 40960K and up)
#
# storage (initial 40960K  next 320K minextents 1 maxextents 99 pctincrease 100
#		freelists 1)
#
storage (initial 20K next 20K minextents 1 maxextents 99 pctincrease 100
		freelists 1)
/
#
# Comments are required on the table and all columns.
# Comments should be in upper and lower case, should be terminated by a
# period, and cannot be longer than 255 bytes.
#
comment on table table_name is
'Table Alias: XXXX.  Add more comment on the table, it can be up to 255 bytes long.'
/
comment on column table_name.column_name is
'This is the comment on this column.  A column comment is required.'
/
comment on column table_name.column_name is
'This is another comment on this column.  A column comment is required.'
/
#
# You may need to create other indexes for performance reasons.
#
create index table_name_n1 on table_name (column_name)
tablespace get_from_dba
pctfree 10
storage (initial 20K next 20K minextents 1 maxextents 99
	pctincrease 100 freelists 1)
/
create index table_name_n2 on table_name (column_name)
tablespace get_from_dba
pctfree 10
storage (initial 20K next 20K minextents 1 maxextents 99
	pctincrease 100 freelists 1)
/
# 
# if this table requires a key assigned by sequence, create the sequence
#
create sequence table_name_pk_sq increment by 1 start with 1
/
# public synonyms may be required
#
create public synonym table_name for table_name
/
create public synonym table_name_pk_sq for table_name_pk_sq
/
#
