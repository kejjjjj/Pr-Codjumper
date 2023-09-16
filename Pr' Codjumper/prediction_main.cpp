#include "pch.hpp"


Prediction::Prediction(const std::string& full_path, const std::string& name, pmove_t* pm, pml_t* pml) :
	project_name(name), project_full_path(full_path)
{

	predicted.push_back(std::move(std::make_unique<PredictionSegment>(pm, pml, pm->cmd.serverTime)));
	current_segment = predicted.front().get();
}
Prediction::Prediction(const std::string& path) : project_full_path(path)
{
	std::cout << "attempting to open file:\n" << path << '\n';
	std::fstream f(project_full_path, static_cast<int>(fs::fileopen::FILE_IN));

	if (!f.is_open()) {
		FatalError(std::format("an error occurred while trying to open \"{}\"!\nreason: {}", project_full_path, fs::get_last_error()));
		return;
	}
	fs::reset();

	IO_ReadVersion(f);

	playerState_s ps	= IO_ReadBlock<playerState_s>(f).value();
	pmove_t pm			= IO_ReadBlock<pmove_t>(f).value();
	pml_t pml			= IO_ReadBlock<pml_t>(f).value();

	pm.ps = &ps;

	//memcpy(pm.ps, &ps, sizeof(playerState_s));

	//std::cout << "serverTime: " << pm.cmd.serverTime << '\n';

	predicted.push_back(std::move(std::make_unique<PredictionSegment>(&pm, &pml, pm.cmd.serverTime)));


	auto current = predicted.back().get();

	current->controller->turntype.reset();
	//current->controller.reset();

	while (!f.eof() && f.good()) {

		IO_ReadController(f, current->controller);
		current->resize_segment(current->controller->numInputs);
		if (f.eof() || !f.good())
			break;

		predicted.push_back(std::move(std::make_unique<PredictionSegment>(nullptr, nullptr, 0)));
		current = predicted.back().get();
	}
	current_segment = predicted.front().get();
	refresh_each_segment();

	f.close();

}
void Prediction::SaveToFile()
{
	std::ofstream o(project_full_path, static_cast<int>(fs::fileopen::FILE_OUT));

	if (!o.is_open()) {
		FatalError(std::format("an error occurred while trying to open \"{}\"!\nreason: {}", project_full_path, fs::get_last_error()));
		return;
	}

	auto entry = predicted.front().get()->initial_state;

	o << TAS_FS_FILEVERSION << '\n';

	IO_WriteData<playerState_s>(o, entry.ps);
	IO_WriteData<pmove_t>(o, entry.pm);
	IO_WriteData<pml_t>(o, entry.pml);
	
	size_t idx = 0;
	for (auto& i : predicted) {
		
		o << "{\n";
		PredictionSegment* segment = i.get();

		 
		IO_WriteController(o, *segment->controller);
		
		o << ((idx == predicted.size()-1) ? "}" : "}\n");


		idx++;

	}

	o.close();

	Com_Printf(CON_CHANNEL_OBITUARY, "project has been saved!\n");
}

template<typename T>
void Prediction::IO_WriteData(std::ofstream& f, const T& data, bool brackets)
{
	DWORD base = (DWORD)&data;
	if(brackets)
		f << '[';
	for (int i = 0; i < sizeof(T); i += 1) {
		std::stringstream ss;
		std::string s;
		ss << std::hex << (int)(*(BYTE*)(base + i));

		if ((s = ss.str()).size() == 1)
			s.insert(s.begin(), '0');

		f << s;

	}
	if (brackets)
		f << "]\n";
}
void Prediction::IO_WriteData(std::ofstream& f, char* const base, char* const end) 
{
	f << '[';

	for (DWORD i = 0; i < end - base; i++) {

		std::stringstream ss;
		std::string s;
		ss << std::hex << (int)(*(BYTE*)(base + i));

		if ((s = ss.str()).size() == 1)
			s.insert(s.begin(), '0');

		f << s;
	}
	f << "]\n";

}
void Prediction::IO_WriteController(std::ofstream& f, prediction_controller& c)
{
	IO_WriteData(f, (char*)&c, (char*)&c.turntype);

	prediction_viewangle_fixed_turn* fixed;
	prediction_viewangle_strafebot* strafebot;


	switch (c.turntype_enum) {
	case prediction_angle_enumerator::FIXED_TURN:
		fixed = dynamic_cast<prediction_viewangle_fixed_turn*>(c.turntype.get());
		IO_WriteData(f, (char*)&fixed->right, (char*)((DWORD)&fixed->up) + sizeof(fixed->up));
		break;
	case prediction_angle_enumerator::STRAFEBOT:
		strafebot = dynamic_cast<prediction_viewangle_strafebot*>(c.turntype.get());
		IO_WriteData(f, (char*)&strafebot->up, (char*)((DWORD)&strafebot->up) + sizeof(strafebot->up));
		break;
	}

	constexpr float ah = 0xcdcc4c3e;
}
void Prediction::IO_ReadVersion(std::fstream& f)
{
	std::string v;
	char ch{};
	bool digit = true;
	while (true) {
		ch = fs::get(f);
		if (!std::isdigit(ch)) {
			break;
		}
		v.push_back(ch);
	}

	if (!IsInteger(v)) {
		FatalError("Corrupted file!\n");
		return;
	}

	int version = std::stoi(v);

	if (version != TAS_FS_FILEVERSION) {
		FatalError("Unsupported file version!\n");
		return;
	}
}
void Prediction::IO_ReadController(std::fstream& f, std::unique_ptr<prediction_controller>& c)
{
	//std::unique_ptr<prediction_controller> c = std::move(std::unique_ptr<prediction_controller>(new prediction_controller));

	char ch = fs::get(f);

	if (ch != '{') {

		FatalError(std::format("std::optional<prediction_controller> Prediction::IO_ReadController(): expected {} instead of {}", '{', ch));
		return;

	}
	fs::get(f); //skip newline

	//read everything until turntype
	IO_ReadBlock(f, size_t((char*)&c->turntype - (char*)c.get()), c.get());

	switch (c->turntype_enum) {
	case prediction_angle_enumerator::FIXED_TURN:
		c->turntype = std::move(std::unique_ptr<prediction_viewangle_fixed_turn>(new prediction_viewangle_fixed_turn()));

		IO_ReadBlock(f, sizeof(prediction_viewangle_fixed_turn)-4, &dynamic_cast<prediction_viewangle_fixed_turn*>(c->turntype.get())->right);
		break;
	case prediction_angle_enumerator::STRAFEBOT:
		c->turntype = std::move(std::unique_ptr<prediction_viewangle_strafebot>(new prediction_viewangle_strafebot()));
		IO_ReadBlock(f, sizeof(prediction_viewangle_strafebot)-4, &dynamic_cast<prediction_viewangle_strafebot*>(c->turntype.get())->up);
		break;
	}
	ch = fs::get(f);
	if (ch != '}') {
		FatalError(std::format("std::optional<prediction_controller> Prediction::IO_ReadController(): expected %c instead of %c", '}', ch));
		return;
	}
	fs::get(f); //skip

	//return c;

}
template<typename T>
std::optional<T> Prediction::IO_ReadBlock(std::fstream& f, size_t amount_of_bytes)
{
	T data{ };
	char ch = fs::get(f);
	if (ch != '[') {
		FatalError(std::format("std::optional<T> Prediction::IO_ReadBlock(): expected {} instead of {}", '[', ch));
		return std::nullopt;
	}

	size_t bytes_read = 0;

	if (!amount_of_bytes)
		amount_of_bytes = sizeof(T);

	DWORD base = (DWORD)(&data);
	
	do {

		std::string hex = "0x";

		for (int i = 0; i < 2; i++) {

			if (f.eof() || !f.good()) {
				FatalError("std::optional<T> Prediction::IO_ReadBlock(): unexpected end of file");
				return std::nullopt;
			}

			ch = fs::get(f);

			if (bytes_read == amount_of_bytes && ch != ']') {
				FatalError(std::format("bytes_read ({}) == sizeof(T) ({}) && ch != ']' ({})", bytes_read, sizeof(T), ch));
				return std::nullopt;
			}
			else if (bytes_read == amount_of_bytes && ch == ']') {
				fs::get(f); //skip the newline
				return data;
			}

			if (!IsHex(ch)) {
				FatalError("std::optional<T> Prediction::IO_ReadBlock(): unexpected end of file");
				return std::nullopt;
			}
			hex.push_back(ch);


		}

		//here it HAS to be from 0 to 255
		auto hex_byte = std::strtol(hex.c_str(), NULL, 0);
		*(BYTE*)base = (BYTE)hex_byte;

		base += 1;
		bytes_read++;

	} while (true);
	
	FatalError("std::optional<T> Prediction::IO_ReadBlock(): unexpected end of file");
	return std::nullopt;
}
void Prediction::IO_ReadBlock(std::fstream& f, size_t amount_of_bytes, void* dst)
{
	char ch = fs::get(f);
	if (ch != '[') {
		FatalError(std::format("std::optional<T> Prediction::IO_ReadBlock(): expected {} instead of {}", '[', ch));
		return;
	}

	size_t bytes_read = 0;

	if (!amount_of_bytes || !dst)
		return;

	DWORD base = (DWORD)(dst);

	do {

		std::string hex = "0x";

		for (int i = 0; i < 2; i++) {

			if (f.eof() || !f.good()) {
				FatalError(std::format("std::optional<T> Prediction::IO_ReadBlock(): unexpected end of file\nwanted to read {} more bytes", amount_of_bytes - bytes_read));
				return;
			}

			ch = fs::get(f);

			if (bytes_read == amount_of_bytes && ch != ']') {
				FatalError(std::format("bytes_read ({}) == sizeof(T) ({}) && ch != ']' ({})", bytes_read, amount_of_bytes, ch));
				return;
			}
			else if (bytes_read == amount_of_bytes && ch == ']') {
				fs::get(f); //skip the newline
				return;
			}

			if (!IsHex(ch)) {
				FatalError(std::format("std::optional<T> Prediction::IO_ReadBlock(): wanted to read a hex decimal instead of {}", ch));
				return;
			}
			hex.push_back(ch);


		}

		//here it HAS to be from 0 to 255
		auto hex_byte = std::strtol(hex.c_str(), NULL, 0);
		*(BYTE*)base = (BYTE)hex_byte;

		base += 1;
		bytes_read++;

	} while (true);

	FatalError("std::optional<T> Prediction::IO_ReadBlock(): unexpected end of file");
}
void Prediction::Hud_Render()
{
	
	Hud_Hitbox();
	std::for_each(predicted.begin(), predicted.end(), [this](std::unique_ptr<PredictionSegment>& seg)
		{
			bool active_segment = current_segment == seg.get();
			seg->Hud_DrawPath(active_segment);

		});


	if (Sys_MilliSeconds() - 5000 >= last_edit_time && last_edit_time != NULL) {
		SaveToFile();
		last_edit_time = NULL;
	}

}
void Prediction::Hud_EngineRender()
{

	std::for_each(predicted.begin(), predicted.end(), [this](std::unique_ptr<PredictionSegment>& seg)
		{
			bool active_segment = current_segment == seg.get();

			if (active_segment)
				seg->Hud_DrawFrameData();

		});
}
void Prediction::Hud_Hitbox()
{
	auto seg = predicted[current_segment_index].get();
	auto data = &seg->cmds[seg->activeFrame];

	if (!data)
		return;

	box_s box(data->origin, data->mins, data->maxs);
	box.R_DrawConstructedBoxEdges(vec4_t{ 255, 0, 0,255 });
	box.R_DrawConstructedBox(vec4_t{ 255, 0, 0,50 });
}
std::list<playback_cmd> Prediction::convert_to_playback()
{
	std::list<playback_cmd> cmd;

	std::for_each(predicted.begin(), predicted.end(), [&cmd](std::unique_ptr<PredictionSegment>& seg) {
		auto cmds = seg->get_data();

		for (const auto& i : cmds) {
			cmd.push_back(i);
		}

	});

	return cmd;
}
void Prediction::add_segment()
{
	auto previous_state = &predicted.back()->state;

	predicted.push_back(std::move(std::make_unique<PredictionSegment>(&previous_state->pm, &previous_state->pml, previous_state->pm.cmd.serverTime)));

	refresh_each_segment();

	current_segment = predicted.back().get();
}
void Prediction::insert_segment()
{
	auto previous_state = &predicted[current_segment_index].get()->state;
	predicted.insert(predicted.begin() + current_segment_index+1, std::move(std::make_unique<PredictionSegment>(&previous_state->pm, &previous_state->pml, previous_state->pm.cmd.serverTime)));

	current_segment = predicted[current_segment_index].get();

	refresh_each_segment();

}
void Prediction::delete_segment()
{
	if (predicted.size() < 2)
		return;

	predicted.erase(predicted.begin() + current_segment_index, predicted.begin() + current_segment_index + 1);
	current_segment_index--;
	current_segment = predicted[current_segment_index].get();
	refresh_each_segment();


}