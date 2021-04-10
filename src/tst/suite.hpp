#pragma once

#include <map>
#include <vector>
#include <sstream>
#include <functional>

#include <utki/debug.hpp>

namespace tst{

class suite{
	friend class tester;
	friend class junit;

	enum status{
		passed,
		failed,
		disabled,
		not_run
	};

	struct test_info{
		std::function<void()> proc;
		status result = status::not_run;
		std::string error_message;
	};

	std::map<std::string, test_info> procedures;

	suite(){}

	size_t num_disabled_tests = 0;

	void add_disabled(const std::string& id);

	void add_disabled(const std::string& id, size_t num_tests);
public:
	suite(suite&&) = default;

	suite(const suite&) = delete;
	suite& operator=(const suite&) = delete;

	size_t size()const noexcept{
		return this->procedures.size();
	}

	size_t num_disabled()const noexcept{
		return this->num_disabled_tests;
	}

	void add(const std::string& id, std::function<void()>&& proc);

	void add_disabled(const std::string& id, std::function<void()>&& proc);

	template <class fixture>
	void add(const std::string& id, std::function<void(fixture&)>&& proc){
		this->add(
				id,
				[proc = std::move(proc)](){
					fixture f;
					proc(f);
				}
			);
	}

	template <class fixture>
	void add_disabled(const std::string& id, std::function<void(fixture&)>&& proc){
		this->add_disabled(id);
	}

	template <class parameter>
	void add(const std::string& id, std::vector<parameter>&& params, const std::function<void(const parameter&)>& proc){
		for(size_t i = 0; i != params.size(); ++i){
			std::stringstream ss;
			ss << id << "[" << i << "]";
			this->add(
					ss.str(),
					[proc = proc, param = std::move(params[i])](){
						ASSERT(proc != nullptr)
						proc(param);
					}
				);
		}
	}

	template <class parameter>
	void add_disabled(const std::string& id, std::vector<parameter>&& params, const std::function<void(const parameter&)>& proc){
		this->add_disabled(id, params.size());
	}

	template <class parameter, class fixture>
	void add(const std::string& id, std::vector<parameter>&& params, const std::function<void(const parameter&, fixture&)>& proc){
		for(size_t i = 0; i != params.size(); ++i){
			std::stringstream ss;
			ss << id << "[" << i << "]";
			this->add(
					ss.str(),
					[proc = proc, param = std::move(params[i])](){
						fixture f(param);
						ASSERT(proc != nullptr)
						proc(param, f);
					}
				);
		}
	}

	template <class parameter, class fixture>
	void add_disabled(const std::string& id, std::vector<parameter>&& params, const std::function<void(const parameter&, fixture&)>& proc){
		this->add_disabled(id, params.size());
	}

	// TODO: persistent fixtures?
};

}
