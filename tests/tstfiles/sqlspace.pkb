REM Script to check space left in database.
REM 
DECLARE
  CURSOR tot_space IS
    SELECT a.tablespace_name, sum(a.bytes) FROM dba_data_files a
      GROUP BY tablespace_name;
  tbl_tot    VARCHAR2(50);
  byt_tot    NUMBER(13);
  CURSOR free_space IS
    SELECT sum(b.bytes) FROM dba_free_space b
      WHERE b.tablespace_name = tbl_tot
      GROUP BY b.tablespace_name;
  byt_free   NUMBER(13);
  pct_used   NUMBER(3);
/* 
 * Here are some useless comments, inserted for testing a2ps
 */
BEGIN
  DBMS_OUTPUT.PUT_LINE (RPAD('TABLESPACE',30,' ') ||  LPAD('Total Bytes',17,' ') || LPAD('Free Bytes',17,' ')
   || LPAD('% Used',10,' '));
  DBMS_OUTPUT.PUT_LINE (RPAD('-',79,'-'));
  LOOP
    IF NOT tot_space%ISOPEN
    THEN
      OPEN tot_space;
    END IF;
    FETCH tot_space INTO
      tbl_tot,
      byt_tot;
    IF tot_space%FOUND
    THEN
      OPEN free_space;
      FETCH free_space INTO byt_free;
      IF free_space%FOUND
      THEN
        pct_used := TRUNC(((1 - (byt_free/byt_tot)) * 100),0);
        DBMS_OUTPUT.PUT_LINE (RPAD(tbl_tot,30,' ') ||  LPAD(byt_tot,17,' ') || LPAD(byt_free,17,' ')
         || LPAD(pct_used,10,' '));
      END IF;
      CLOSE free_space;
    ELSE
      EXIT;
    END IF;
  END LOOP;
  CLOSE tot_space;
END;
/
