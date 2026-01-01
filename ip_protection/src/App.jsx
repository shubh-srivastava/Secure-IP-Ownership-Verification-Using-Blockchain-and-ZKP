import { BrowserRouter, Routes, Route } from "react-router-dom";
import Navbar from "./components/Navbar";

import Dashboard from "./pages/Dashboard";
import RegisterUser from "./pages/RegisterUser";
import RegisterIP from "./pages/RegisterIP";
import VerifyIP from "./pages/VerifyIP";
import Blockchain from "./pages/Blockchain";

export default function App() {
  return (
    <BrowserRouter>
      <Navbar />
      <Routes>
        <Route path="/" element={<Dashboard />} />
        <Route path="/register" element={<RegisterUser />} />
        <Route path="/create" element={<RegisterIP />} />
        <Route path="/verify" element={<VerifyIP />} />
        <Route path="/chain" element={<Blockchain />} />
      </Routes>
    </BrowserRouter>
  );
}
