#include "Modal.hpp"
#include "Logger.hpp"
#include "utils.hpp"

#include <json.hpp>

TextInput::TextInput(std::string const& custom_id, std::string const& label, int style) :
	_custom_id(custom_id), _label(label), _style(style) {}

json TextInput::DumpToJson() const
{
	json j = json::object();
	j["type"] = 4;
	j["custom_id"] = _custom_id;
	j["style"] = _style;
	j["label"] = _label;
	j["min_length"] = _min_length;
	j["max_length"] = _max_length;
	j["required"] = _required;
	if (!_placeholder.empty())
		j["placeholder"] = _placeholder;
	if (!_default_value.empty())
		j["value"] = _default_value;
	return j;
}

Modal::Modal(std::string const& custom_id, std::string const& title) :
	_custom_id(custom_id), _title(title) {}

TextInputId_t Modal::AddTextInput(std::string const& custom_id, std::string const& label,
	int style, std::string const& placeholder, std::string const& default_value,
	int min_length, int max_length, bool required)
{
	TextInputId_t id = _next_input_id++;
	auto input = TextInput_t(new TextInput(custom_id, label, style));
	input->SetPlaceholder(placeholder);
	input->SetDefaultValue(default_value);
	input->SetMinLength(min_length);
	input->SetMaxLength(max_length);
	input->SetRequired(required);
	_inputs.emplace(id, std::move(input));
	return id;
}

json Modal::DumpToJson() const
{
	json j = json::object();
	j["type"] = 9; // MODAL
	json data = json::object();
	data["custom_id"] = _custom_id;
	data["title"] = _title;
	data["components"] = json::array();

	for (auto const& input_pair : _inputs)
	{
		auto const& input = input_pair.second;
		json row = json::object();
		row["type"] = 1;
		row["components"] = json::array();
		row["components"] += input->DumpToJson();
		data["components"] += row;
	}

	j["data"] = data;
	return j;
}

// TextInputManager
TextInputId_t TextInputManager::AddTextInput(std::string const& custom_id,
	std::string const& label, int style)
{
	TextInputId_t id = 1;
	while (m_Inputs.find(id) != m_Inputs.end())
		++id;

	if (!m_Inputs.emplace(id, TextInput_t(new TextInput(custom_id, label, style))).first->second)
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"can't create text input: duplicate key '{}'", id);
		return INVALID_TEXT_INPUT_ID;
	}

	Logger::Get()->Log(samplog_LogLevel::INFO,
		"successfully created text input with id '{}'", id);
	return id;
}

bool TextInputManager::DeleteTextInput(TextInputId_t id)
{
	if (m_Inputs.find(id) == m_Inputs.end())
	{
		Logger::Get()->Log(samplog_LogLevel::WARNING,
			"attempted to delete text input '{}' but it does not exist", id);
		return false;
	}
	m_Inputs.erase(id);
	return true;
}

TextInput_t const& TextInputManager::FindTextInput(TextInputId_t id)
{
	static TextInput_t invalid_input;
	auto it = m_Inputs.find(id);
	if (it == m_Inputs.end())
		return invalid_input;
	return it->second;
}

// ModalManager
ModalId_t ModalManager::AddModal(std::string const& custom_id, std::string const& title)
{
	ModalId_t id = 1;
	while (m_Modals.find(id) != m_Modals.end())
		++id;

	if (!m_Modals.emplace(id, Modal_t(new Modal(custom_id, title))).first->second)
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"can't create modal: duplicate key '{}'", id);
		return INVALID_MODAL_ID;
	}

	Logger::Get()->Log(samplog_LogLevel::INFO,
		"successfully created modal with id '{}'", id);
	return id;
}

bool ModalManager::DeleteModal(ModalId_t id)
{
	if (m_Modals.find(id) == m_Modals.end())
	{
		Logger::Get()->Log(samplog_LogLevel::WARNING,
			"attempted to delete modal '{}' but it does not exist", id);
		return false;
	}
	m_Modals.erase(id);
	return true;
}

Modal_t const& ModalManager::FindModal(ModalId_t id)
{
	static Modal_t invalid_modal;
	auto it = m_Modals.find(id);
	if (it == m_Modals.end())
		return invalid_modal;
	return it->second;
}
