#include <windows.h>
#include <iostream>
#include <speechapi_cxx.h>

using namespace std;
using namespace Microsoft::CognitiveServices::Speech;

void SpeechContinuousRecognition()
{
	// サブスクリプションキーとサービスリージョンで Speech 構成のインスタンスを作成します
	// 下記の自分のサブスクリプションキーとサービスリージョンに編集しましょう (例 "XXXXX"->"japanwest")
	auto config = SpeechConfig::FromSubscription( "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", "XXXXX" );

	// 認識させたい言語を指定します (日本語="ja-JP", 英語="en-US")
	auto fromLanguage = "ja-JP";
	config->SetSpeechRecognitionLanguage( fromLanguage );

	// 上記の設定からスピーチ認識を作成します
	auto recognizer = SpeechRecognizer::FromConfig( config );

	cout << u8"何か話してみてください\n";

	// 認識終了の同期を担保します
	promise<void> recognitionEnd;

	// イベントに申請します
	recognizer->Recognizing.Connect( []( const SpeechRecognitionEventArgs& e )
	{
		cout << u8"\r認識中..:" << e.Result->Text << std::flush;
	});

	recognizer->Recognized.Connect( []( const SpeechRecognitionEventArgs& e )
	{
		if ( e.Result->Reason == ResultReason::RecognizedSpeech )
		{
			cout << u8"\r認識完了: " << e.Result->Text << "\n" << std::endl;
		}
		else if ( e.Result->Reason == ResultReason::NoMatch )
		{
			cout << u8"NOMATCH: 声を認識できませんでした" << std::endl;
		}
	});

	recognizer->Canceled.Connect( [&recognitionEnd]( const SpeechRecognitionCanceledEventArgs& e )
	{
		cout << u8"CANCELED: 理由=" << (int) e.Reason << std::endl;

		if ( e.Reason == CancellationReason::Error )
		{
			cout << u8"CANCELED: エラーコード= " << (int) e.ErrorCode << std::endl;
			cout << u8"CANCELED: エラー詳細= " << e.ErrorDetails << std::endl;
			cout << u8"CANCELED: サブスクリプション情報を確認しましょう" << std::endl;

			// 認識停止を通知します
			recognitionEnd.set_value();
		}
	});

	// 連続的な認識を開始します (停止するには StopContinuousRecognitionAsync() を使います)
	recognizer->StartContinuousRecognitionAsync().get();

	cout << u8"Enter キーを押すと停止します\n" << std::endl;
	cin.get();

	// 認識を停止します
	recognizer->StopContinuousRecognitionAsync().get();
}

int wmain()
{
	SpeechContinuousRecognition();

	return 0;
}
