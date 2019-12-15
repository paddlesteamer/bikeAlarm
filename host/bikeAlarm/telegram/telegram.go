package telegram

import  (
	"fmt"
	"net/http"
)

var config struct {
    token  string ""
    chatId string ""
}

func Init(token string, chatId string) {
    config.token = token
    config.chatId = chatId
}

func SendMessage(message string) (bool) {
    _, err := http.Get(fmt.Sprintf("https://api.telegram.org/bot%s/sendMessage?chat_id=%s&text=%s",
        config.token, config.chatId, message))

    return err == nil
}



