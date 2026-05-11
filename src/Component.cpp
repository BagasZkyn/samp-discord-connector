#include "Component.hpp"
#include "Logger.hpp"
#include "utils.hpp"

#include <json.hpp>

json Button::DumpToJson() const
{
	json j = json::object();
	j["type"] = 2;
	j["style"] = _style;
	j["custom_id"] = _custom_id;
	j["label"] = _label;
	j["disabled"] = _disabled;

	if (!_url.empty())
	{
		j["url"] = _url;
		j["style"] = 5; // link buttons force style 5
	}

	if (!_emoji_name.empty())
	{
		json emoji = json::object();
		emoji["name"] = _emoji_name;
		if (!_emoji_id.empty())
			emoji["id"] = _emoji_id;
		j["emoji"] = emoji;
	}

	return j;
}

SelectMenu::SelectMenu(std::string const& custom_id, std::string const& placeholder,
	int min_values, int max_values) :
	_custom_id(custom_id), _placeholder(placeholder),
	_min_values(min_values), _max_values(max_values) {}

void SelectMenu::AddOption(std::string const& label, std::string const& value,
	std::string const& description, std::string const& emoji_name,
	std::string const& emoji_id)
{
	_options.emplace_back(new SelectOption(label, value, description, emoji_name, emoji_id));
}

json SelectMenu::DumpToJson() const
{
	json j = json::object();
	j["type"] = 3;
	j["custom_id"] = _custom_id;
	j["placeholder"] = _placeholder;
	j["min_values"] = _min_values;
	j["max_values"] = _max_values;
	j["disabled"] = _disabled;

	j["options"] = json::array();
	for (auto const& opt : _options)
	{
		json o = json::object();
		o["label"] = opt->_label;
		o["value"] = opt->_value;
		if (!opt->_description.empty())
			o["description"] = opt->_description;
		if (!opt->_emoji_name.empty())
		{
			json emoji = json::object();
			emoji["name"] = opt->_emoji_name;
			if (!opt->_emoji_id.empty())
				emoji["id"] = opt->_emoji_id;
			o["emoji"] = emoji;
		}
		j["options"] += o;
	}

	return j;
}

void ActionRow::AddButton(ButtonId_t button)
{
	_buttons.push_back(button);
}

void ActionRow::AddSelectMenu(SelectMenuId_t menu)
{
	_select_menu_id = menu;
}

bool ActionRow::CanAddComponent() const
{
	if (_select_menu_id != INVALID_SELECT_MENU_ID)
		return false;
	return _buttons.size() < 5;
}

json ActionRow::DumpToJson() const
{
	json j = json::object();
	j["type"] = 1; // Action Row
	j["components"] = json::array();

	if (_select_menu_id != INVALID_SELECT_MENU_ID)
	{
		auto const& menu = SelectMenuManager::Get()->FindSelectMenu(_select_menu_id);
		if (menu)
			j["components"] += menu->DumpToJson();
	}

	for (auto const& btn_id : _buttons)
	{
		auto const& btn = ButtonManager::Get()->FindButton(btn_id);
		if (btn)
			j["components"] += btn->DumpToJson();
	}

	return j;
}

// ButtonManager
ButtonId_t ButtonManager::AddButton(std::string const& custom_id,
	std::string const& label, int style)
{
	ButtonId_t id = 1;
	while (m_Buttons.find(id) != m_Buttons.end())
		++id;

	if (!m_Buttons.emplace(id, Button_t(new Button(custom_id, label, style))).first->second)
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"can't create button: duplicate key '{}'", id);
		return INVALID_BUTTON_ID;
	}

	Logger::Get()->Log(samplog_LogLevel::INFO,
		"successfully created button with id '{}'", id);
	return id;
}

bool ButtonManager::DeleteButton(ButtonId_t id)
{
	if (m_Buttons.find(id) == m_Buttons.end())
	{
		Logger::Get()->Log(samplog_LogLevel::WARNING,
			"attempted to delete button '{}' but it does not exist", id);
		return false;
	}
	m_Buttons.erase(id);
	return true;
}

Button_t const& ButtonManager::FindButton(ButtonId_t id)
{
	static Button_t invalid_button;
	auto it = m_Buttons.find(id);
	if (it == m_Buttons.end())
		return invalid_button;
	return it->second;
}

// SelectMenuManager
SelectMenuId_t SelectMenuManager::AddSelectMenu(std::string const& custom_id,
	std::string const& placeholder, int min_values, int max_values)
{
	SelectMenuId_t id = 1;
	while (m_Menus.find(id) != m_Menus.end())
		++id;

	if (!m_Menus.emplace(id, SelectMenu_t(new SelectMenu(custom_id, placeholder, min_values, max_values))).first->second)
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"can't create select menu: duplicate key '{}'", id);
		return INVALID_SELECT_MENU_ID;
	}

	Logger::Get()->Log(samplog_LogLevel::INFO,
		"successfully created select menu with id '{}'", id);
	return id;
}

bool SelectMenuManager::DeleteSelectMenu(SelectMenuId_t id)
{
	if (m_Menus.find(id) == m_Menus.end())
	{
		Logger::Get()->Log(samplog_LogLevel::WARNING,
			"attempted to delete select menu '{}' but it does not exist", id);
		return false;
	}
	m_Menus.erase(id);
	return true;
}

SelectMenu_t const& SelectMenuManager::FindSelectMenu(SelectMenuId_t id)
{
	static SelectMenu_t invalid_menu;
	auto it = m_Menus.find(id);
	if (it == m_Menus.end())
		return invalid_menu;
	return it->second;
}

json SelectMenuManager::DumpOptionsToJson(std::vector<SelectOption_t> const& options)
{
	json arr = json::array();
	for (auto const& opt : options)
	{
		json o = json::object();
		o["label"] = opt->_label;
		o["value"] = opt->_value;
		if (!opt->_description.empty())
			o["description"] = opt->_description;
		if (!opt->_emoji_name.empty())
		{
			json emoji = json::object();
			emoji["name"] = opt->_emoji_name;
			if (!opt->_emoji_id.empty())
				emoji["id"] = opt->_emoji_id;
			o["emoji"] = emoji;
		}
		arr += o;
	}
	return arr;
}

// ActionRowManager
ActionRowId_t ActionRowManager::AddActionRow()
{
	ActionRowId_t id = 1;
	while (m_Rows.find(id) != m_Rows.end())
		++id;

	if (!m_Rows.emplace(id, ActionRow_t(new ActionRow())).first->second)
	{
		Logger::Get()->Log(samplog_LogLevel::ERROR,
			"can't create action row: duplicate key '{}'", id);
		return INVALID_ACTION_ROW_ID;
	}

	Logger::Get()->Log(samplog_LogLevel::INFO,
		"successfully created action row with id '{}'", id);
	return id;
}

bool ActionRowManager::DeleteActionRow(ActionRowId_t id)
{
	if (m_Rows.find(id) == m_Rows.end())
	{
		Logger::Get()->Log(samplog_LogLevel::WARNING,
			"attempted to delete action row '{}' but it does not exist", id);
		return false;
	}
	m_Rows.erase(id);
	return true;
}

ActionRow_t const& ActionRowManager::FindActionRow(ActionRowId_t id)
{
	static ActionRow_t invalid_row;
	auto it = m_Rows.find(id);
	if (it == m_Rows.end())
		return invalid_row;
	return it->second;
}

json ActionRowManager::DumpComponentsToJson(std::vector<ActionRowId_t> const& rows)
{
	json arr = json::array();
	for (auto const& row_id : rows)
	{
		auto const& row = ActionRowManager::Get()->FindActionRow(row_id);
		if (row)
			arr += row->DumpToJson();
	}
	return arr;
}
