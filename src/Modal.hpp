#pragma once

#include "Singleton.hpp"
#include "types.hpp"

#include <string>
#include <vector>
#include <atomic>
#include <json.hpp>

using json = nlohmann::json;

class TextInput
{
public:
	TextInput(std::string const& custom_id, std::string const& label, int style);
	~TextInput() = default;

	void SetPlaceholder(std::string const& placeholder) { _placeholder = placeholder; }
	void SetDefaultValue(std::string const& value) { _default_value = value; }
	void SetMinLength(int min_length) { _min_length = min_length; }
	void SetMaxLength(int max_length) { _max_length = max_length; }
	void SetRequired(bool required) { _required = required; }

	json DumpToJson() const;

private:
	std::string _custom_id;
	std::string _label;
	int _style;
	std::string _placeholder;
	std::string _default_value;
	int _min_length = 0;
	int _max_length = 4000;
	bool _required = true;
};

class Modal
{
public:
	Modal(std::string const& custom_id, std::string const& title);
	~Modal() = default;

	TextInputId_t AddTextInput(std::string const& custom_id, std::string const& label,
		int style, std::string const& placeholder, std::string const& default_value,
		int min_length, int max_length, bool required);
	void SetTitle(std::string const& title) { _title = title; }

	std::string const& GetCustomId() const { return _custom_id; }
	std::string const& GetTitle() const { return _title; }

	json DumpToJson() const;

private:
	std::string _custom_id;
	std::string _title;
	std::map<TextInputId_t, TextInput_t> _inputs;
	TextInputId_t _next_input_id = 1;
};

class TextInputManager : public Singleton<TextInputManager>
{
	friend class Singleton<TextInputManager>;
private:
	TextInputManager() = default;
	~TextInputManager() = default;

private:
	std::map<TextInputId_t, TextInput_t> m_Inputs;

public:
	TextInputId_t AddTextInput(std::string const& custom_id, std::string const& label, int style);
	bool DeleteTextInput(TextInputId_t id);
	TextInput_t const& FindTextInput(TextInputId_t id);
};

class ModalManager : public Singleton<ModalManager>
{
	friend class Singleton<ModalManager>;
private:
	ModalManager() = default;
	~ModalManager() = default;

private:
	std::map<ModalId_t, Modal_t> m_Modals;

public:
	ModalId_t AddModal(std::string const& custom_id, std::string const& title);
	bool DeleteModal(ModalId_t id);
	Modal_t const& FindModal(ModalId_t id);
};
