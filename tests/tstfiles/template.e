class TEMPLATE2 inherit

   ARGUMENTS
      undefine
	 copy, consistent, is_equal, setup
      end

   TEMPLATE2_LEX
      redefine
	 store_analyzer,
	 retrieve_analyzer
      end

creation

   make

feature

   root_line: TEMPLATE_CLAUSE;

   analyzer_file_name: STRING is "/inf/nana/infthes/demaille/eiffel/template2/lexical/template2_analyzer";

   make is
      local
	 text_name: STRING;
	 analyzer_file: RAW_FILE;
	 analyzer_needs_storing: BOOLEAN;
	 text_file: PLAIN_TEXT_FILE;
	 argument_position: INTEGER
      do
	 if argument_count = 0 then
	    io.putstring ("Usage: template2 file_name%N");
	 else
	    !! root_line.make;
	    io.putstring (Separator_line_message);
	    io.putstring ("TEMPLATE2");
	    io.putstring (" (Version 1.0).	 (C)");
	    !! analyzer_file.make (analyzer_file_name);
	    if analyzer_file.exists then
	       io.putstring ("%NRetrieving analyzer . . .");
	       retrieve_analyzer (analyzer_file_name);
	       root_line.document.set_lexical (analyzer);
	       io.putstring ("DONE");
	    else
	       io.putstring ("%NBuilding analyzer ... ");
	       build (root_line.document);
	       analyzer_needs_storing := true
	       io.putstring ("DONE");
	    end;
	    
	    -- Test for left-recursion ?
	    -- test_left_recursion;
	    
	    text_name := argument (1);
	    !! text_file.make (text_name);
	    if (not text_file.exists) then
	       io.putstring (text_name);
	       io.putstring (": No such file or directory%N");
	    else
	       root_line.document.set_input_file (text_name);
	       root_line.document.get_token;
	       io.putstring ("%NParsing document in file: ")
	       io.putstring (text_name);
	       io.putstring (" ... ");
	       root_line.parse;
	       if root_line.parsed then
		  io.putstring ("DONE");
		  io.putstring ("%NParse tree ... %N");
		  root_line.display (io.output);
		  io.putstring ("%NApplying semantics ... ");
		  root_line.semantics;
		  io.putstring ("DONE%N");
	       else
		  io.putstring ("%NSyntax error with document");
	       end;
	    end
	    if analyzer_needs_storing then
	       io.putstring ("%NStoring analyzer ... ");
	       -- store_analyzer (analyzer_file_name);
	       io.putstring ("DONE%N");
	       io.putstring (Separator_line_message);
	    end;
	    io.new_line;
	 end;
      end; -- make
   Separator_line_message: STRING is "---------------------------------------------------------------------------%N";

   test_left_recursion is
	 -- Test root_line for left recursion.
      local
	 t_b: BOOLEAN;
      do
	 root_line.print_mode.put (true);
	 root_line.expand_all;
	 t_b := not root_line.left_recursion;
	 root_line.check_recursion;
	 if not root_line.left_recursion.item then
	    io.putstring ("No left recursion detected%N");
	 else
	    io.putstring ("Left recursive.%N");
	 end;
	 
      end; -- test_left_recursion

   store_analyzer (file_name: STRING) is
	 -- Store `analyzer' in file named `file_name'.
      require else
	 initialized: initialized
      local
	 store_file: RAW_FILE
      do
	 if analyzer = Void then
	    !! analyzer.make
	 end;
	 !! store_file.make_open_write (file_name);
	 analyzer.basic_store (store_file);
	 store_file.close;
      end; -- store_analyzer

   retrieve_analyzer (file_name: STRING) is
	 --  Retrieve `analyzer' from file named `file_name'.
      local
	 retrieved_file: RAW_FILE
      do
	 if analyzer = Void then
	    !! analyzer.make
	 end;
	 !! retrieved_file.make_open_read (file_name);
	 analyzer ?= analyzer.retrieved (retrieved_file);
	 retrieved_file.close;
      end; -- store_analyzer_feature

end -- class TEMPLATE2
