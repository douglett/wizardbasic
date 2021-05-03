#pragma once


// ----------------------------------------
// Output method B (structure building)
// ----------------------------------------


enum PSTATE_T {
	PS_NONE = 0,
	PS_EXPRESSION,
	PS_VARPATH,
	PS_STRUCT,
	PS_DIM,
	PS_FUNCTION,
	PS_STMT_BLOCK,
	PS_STMT_PRINT,
	PS_STMT_INPUT,
	PS_STMT_IF,
	PS_STMT_WHILE,
	PS_STMT_RETURN,
	PS_STMT_CALL,
	PS_STMT_SET,
	PS_STMT_LET,
};


struct wb_dim {
	string type, id;
	bool isarray;
};
struct wb_dimshort {
	string type, id;
	bool isarray;
};
struct wb_struct {
	string id;
	vector<wb_dimshort> members;
};
struct wb_function {
	string id;
	vector<wb_dimshort> args;
};
struct wb_stmt_block {
	struct stmt { string type; int id; };
	vector<stmt> stmts;
};
struct wb_stmt_print {
	struct arg { string type; int id; };
	vector<arg> list;
};
struct wb_stmt_input {
	int prompt;
	int varpath;
};
struct wb_stmt_if {
	struct ifthen { int cond; int block; };
	vector<ifthen> ifthens;
};
struct wb_stmt_while {
	int cond, block;
};
struct wb_stmt_return {
	int expression;
};
struct wb_stmt_call {
	string id;
	vector<int> args;
};
struct wb_stmt_set {
	int varpath_dest, varpath_source;
};
struct wb_stmt_let {
	int varpath, expression;
};
struct wb_varpath {
	vector<string> list;
};
struct wb_expression {
	vector<string> list;
};



struct OutputB : Output {
	struct pstate {
		PSTATE_T pstate;
		int id;
	};
	vector<pstate>           state;

	vector<wb_struct>        structs;
	vector<wb_dim>           dims;
	vector<wb_function>      funcs;
	vector<wb_stmt_print>    prints;
	vector<wb_stmt_input>    inputs;
	vector<wb_stmt_if>       ifs;
	vector<wb_stmt_while>    whiles;
	vector<wb_stmt_return>   returns;
	vector<wb_stmt_call>     calls;
	vector<wb_stmt_set>      sets;
	vector<wb_stmt_let>      lets;
	vector<wb_stmt_block>    blocks;
	vector<wb_varpath>       varpaths;
	// vector<wb_expression>    expressions = { {{"i 0"}}, {{"i 1"}} };
	vector<wb_expression>    expressions;
	vector<string>           literals;


	PSTATE_T curstate()  { return state.size() ? state.back().pstate : PS_NONE; }
	int      curid()     { return state.size() ? state.back().id     : -1; }
	void     statewarn() { printf("<statewarn %d>   ", curstate()); }


	void struct_start(const string& id) {
		structs.push_back({ id });
		state.push_back({ PS_STRUCT, int(structs.size()-1) });
	}
	void struct_end() {
		state.pop_back();
	}
	void dim_short(const string& type, const string& id, bool isarray) {
		if      (curstate() == PS_STRUCT)    structs.at(curid()).members.push_back({ type, id, isarray });
		else if (curstate() == PS_FUNCTION)  funcs.at(curid()).args.push_back({ type, id, isarray });
		else    statewarn(),  Output::dim_short(type, id, isarray);
	}
	void dim_start(const string& type, const string& id) {
		dims.push_back({ type, id });
		state.push_back({ PS_DIM, int(dims.size()-1) });
	}
	void dim_isarray(bool val) {
		dims.at(curid()).isarray = val;
	}
	void dim_end() {
		state.pop_back();
	}
	void func_start(const string& id) {
		funcs.push_back({ id });
		state.push_back({ PS_FUNCTION, int(funcs.size()-1) });
	}
	void func_end() {
		state.pop_back();
	}


	void block_start() {
		blocks.push_back({});
		int idx = blocks.size() - 1;
		if      (curstate() == PS_STMT_IF)     ifs.at(curid()).ifthens.back().block = idx;
		else if (curstate() == PS_STMT_WHILE)  whiles.at(curid()).block = idx;
		else    statewarn(),  Output::block_start();
		state.push_back({ PS_STMT_BLOCK, idx });
	}
	void _block_append_stmt(const string& type, int id) {
		// helper - append statement to block.
		if    (curstate() == PS_STMT_BLOCK)  blocks.at(curid()).stmts.push_back({ type, id });
		else  statewarn(),  printf("(%s append)\n", type.c_str());
	}
	void block_end() {
		state.pop_back();
	}	
	void print_start() {
		prints.push_back({});
		state.push_back({ PS_STMT_PRINT, int(prints.size()-1) });
	}
	void print_end() {
		state.pop_back();
	}
	void input_start() {
		int prompt = _string_getindex("> ");
		inputs.push_back({ prompt, -1 });
		_block_append_stmt("input", inputs.size()-1);
		state.push_back({ PS_STMT_INPUT, int(inputs.size()-1) });
	}
	void input_end() {
		state.pop_back();
	}
	void if_start() {
		ifs.push_back({ {{ -1, -1 }} });
		_block_append_stmt("if", ifs.size()-1);
		state.push_back({ PS_STMT_IF, int(ifs.size()-1) });
	}
	void if_end() {
		state.pop_back();
	}
	void while_start() {
		whiles.push_back({});
		_block_append_stmt("while", whiles.size()-1);
		state.push_back({ PS_STMT_WHILE, int(whiles.size()-1) });
	}
	void while_end() {
		state.pop_back();
	}
	void return_start() {
		returns.push_back({});
		_block_append_stmt("return", returns.size()-1);
		state.push_back({ PS_STMT_RETURN, int(returns.size()-1) });
	}
	void return_end() {
		state.pop_back();
	}
	void call_start(const string& id) {
		calls.push_back({ id });
		_block_append_stmt("call", calls.size()-1);
		state.push_back({ PS_STMT_CALL, int(calls.size()-1) });
	}
	void call_end() {
		state.pop_back();
	}
	void set_start() {
		sets.push_back({ -1, -1 });
		_block_append_stmt("set", sets.size()-1);
		state.push_back({ PS_STMT_SET, int(sets.size()-1) });
	}
	void set_end() {
		state.pop_back();
	}
	void let_start() {
		lets.push_back({});
		_block_append_stmt("let", lets.size()-1);
		state.push_back({ PS_STMT_LET, int(lets.size()-1) });
	}
	void let_end() {
		state.pop_back();
	}


	void string_literal(const string& lit) {
		int idx = _string_getindex(lit);
		if      (curstate() == PS_STMT_PRINT)   prints.at(curid()).list.push_back({ "lit", idx });
		else if (curstate() == PS_STMT_INPUT)   inputs.at(curid()).prompt = idx;
		else    statewarn(),  Output::string_literal(lit);
	}
	int _string_getindex(const string& lit) {
		// helper - de-dup literals
		for (int i = 0; i < literals.size(); i++)
			if (literals[i] == lit)  return i;
		literals.push_back(lit);
		return literals.size() - 1;
	}
	void ex_start() {
		expressions.push_back({});
		int idx = expressions.size() - 1;
		if      (curstate() == PS_STMT_IF)      ifs.at(curid()).ifthens.back().cond = idx;
		else if (curstate() == PS_STMT_WHILE)   whiles.at(curid()).cond = idx;
		else if (curstate() == PS_STMT_RETURN)  returns.at(curid()).expression = idx;
		else if (curstate() == PS_STMT_LET)     lets.at(curid()).expression = idx;
		else if (curstate() == PS_STMT_CALL)    calls.at(curid()).args.push_back(idx);
		else if (curstate() == PS_VARPATH)      varpaths.at(curid()).list.push_back("expr $" + to_string(idx));
		else if (curstate() == PS_STMT_PRINT)   prints.at(curid()).list.push_back({ "expr", idx });
		else    statewarn(),  Output::ex_start();
		state.push_back({ PS_EXPRESSION, idx });
	}
	void ex_push(const string& ex) {
		expressions.at(curid()).list.push_back(ex);
	}
	void ex_end() {
		state.pop_back();
	}
	void varpath_start(const string& id) {
		varpaths.push_back({ {id} });
		int idx = varpaths.size() - 1;
		if      (curstate() == PS_EXPRESSION)   expressions.at(curid()).list.push_back("varpath "+to_string(idx));
		else if (curstate() == PS_STMT_LET)     lets.at(curid()).varpath = idx;
		else if (curstate() == PS_STMT_INPUT)   inputs.at(curid()).varpath = idx;
		else if (curstate() == PS_STMT_SET) {
			if      (sets.at(curid()).varpath_dest == -1) sets.at(curid()).varpath_dest = idx;
			else if (sets.at(curid()).varpath_source == -1) sets.at(curid()).varpath_source = idx;
		}
		else    statewarn(),  Output::varpath_start(id);
		state.push_back({ PS_VARPATH, idx });
	}
	void varpath_push(const string& path) {
		varpaths.at(curid()).list.push_back(path);
	}
	void varpath_end() {
		state.pop_back();
	}


	void show() {
		// program control data
		printf(":structs:      $%d\n", structs.size());
		for (int i = 0; i < structs.size(); i++) {
			printf("  %s\n", structs[i].id.c_str());
			for (const auto& d : structs[i].members)
				printf("\t%s %s %s\n", d.type.c_str(), d.id.c_str(), (d.isarray ? "[]" : ""));
		}
		printf(":dims:         $%d\n", dims.size());
		for (int i = 0; i < dims.size(); i++) {
			printf("  $%d  %s %s %s\n", i, dims[i].type.c_str(), dims[i].id.c_str(), (dims[i].isarray ? "[]" : ""));
		}
		printf(":funcs:        $%d\n", funcs.size());
		for (int i = 0; i < funcs.size(); i++) {
			printf("  %s\n", funcs[i].id.c_str());
			for (const auto& d : funcs[i].args)
				printf("\t%s %s %s\n", d.type.c_str(), d.id.c_str(), (d.isarray ? "[]" : ""));
		}


		// statements
		printf(":prints:       $%d\n", prints.size());
		for (int i = 0; i < prints.size(); i++) {
			// printf("  $%d  ", i);
			// for (const auto& st : prints[i].list)
			// 	printf("%s $%d, ", st.type.c_str(), st.id);
			// printf("\n");
			printf("  $%d\n", i);
			for (const auto& st : prints[i].list)
				printf("\t%s $%d\n", st.type.c_str(), st.id);
		}
		printf(":inputs:       $%d\n", inputs.size());
		for (int i = 0; i < inputs.size(); i++) {
			printf("  $%d \tprompt $%d, varpath $%d\n", i, inputs[i].prompt, inputs[i].varpath);
		}
		printf(":ifs:          $%d\n", ifs.size());
		for (int i = 0; i < ifs.size(); i++) {
			// printf("  $%d  ", i);
			// for (const auto& it : ifs[i].ifthens)
			// 	printf("(cond $%d, block $%d)  ", it.cond, it.block);
			// printf("\n");
			printf("  $%d\n", i);
			for (const auto& it : ifs[i].ifthens)
				printf("\tcond $%d, block $%d\n", it.cond, it.block);
		}
		printf(":whiles:       $%d\n", whiles.size());
		for (int i = 0; i < whiles.size(); i++) {
			printf("  $%d \tcond $%d, block $%d\n", i, whiles[i].cond, whiles[i].block);
		}
		printf(":returns:      $%d\n", returns.size());
		for (int i = 0; i < returns.size(); i++) {
			printf("  $%d \texpr $%d\n", i, returns[i].expression);
		}
		printf(":calls:        $%d\n", calls.size());
		for (int i = 0; i < calls.size(); i++) {
			// printf("  $%d  %s  ( ", i, calls[i].id.c_str());
			// for (const auto& a : calls[i].args)
			// 	printf("$%d, ", a);
			// printf(" )\n");
			printf("  $%d  %s\n", i, calls[i].id.c_str());
			for (const auto& a : calls[i].args)
				printf("\texpr $%d\n", a);
		}
		printf(":sets:         $%d\n", sets.size());
		for (int i = 0; i < sets.size(); i++) {
			printf("  $%d \tvarpath_dest $%d, varpath_source $%d\n", i, sets[i].varpath_dest, sets[i].varpath_source);
		}
		printf(":lets:         $%d\n", lets.size());
		for (int i = 0; i < lets.size(); i++) {
			printf("  $%d \tvarpath $%d, expr $%d\n", i, lets[i].varpath, lets[i].expression);
		}


		// statement blocks
		printf(":blocks:       $%d\n", blocks.size());
		for (int i = 0; i < blocks.size(); i++) {
			printf("  $%d\n", i);
			for (const auto& s : blocks[i].stmts)
				printf("\t%s $%d\n", s.type.c_str(), s.id);
		}


		// data
		printf(":varpaths:     $%d\n", varpaths.size());
		for (int i = 0; i < varpaths.size(); i++) {
			printf("  $%d \t", i);
			for (const auto& l : varpaths[i].list)
				printf("%s  ", l.c_str());
			printf("\n");
		}
		printf(":expressions:  $%d\n", expressions.size());
		for (int i = 0; i < expressions.size(); i++) {
			printf("  $%d \t", i);
			for (const auto& l : expressions[i].list)
				printf("%s  ", l.c_str());
			printf("\n");
		}
		printf(":literals:     $%d\n", literals.size());
		for (int i = 0; i < literals.size(); i++) {
			printf("  $%d \t\"%s\"\n", i, literals[i].c_str());
		}
	}
};