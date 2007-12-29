SELECT H.emp_no, 
       H.hist_type,
       count(*)  
FROM   emp_history H
GROUP  BY H.emp_no, H.hist_type;
